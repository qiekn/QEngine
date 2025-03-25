import os
import re
import glob
import json
from pathlib import Path

def parse_header(file_path):
    with open(file_path, 'r') as f:
        content = f.read()
    
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    
    lines = content.split('\n')
    cleaned_lines = []
    
    for line in lines:
        comment_pos = line.find('//')
        if comment_pos >= 0:
            line = line[:comment_pos]
        cleaned_lines.append(line)
    
    content = '\n'.join(cleaned_lines)
    
    content = re.sub(r'enum\s+class\s+\w+\s*{[^}]*}', '', content, flags=re.DOTALL)
    
    variant_match = re.search(r'VARIANT\((\w+)\)', content)
    if not variant_match:
        return None
    
    class_match = re.search(r'(struct|class)\s+(\w+)\s*(?::\s*public\s+(\w+))?', content)
    if not class_match:
        return None
    
    class_name = class_match.group(2)
    
    if class_name in ["VariantCreateInfo", "VariantBase"]:
        return None
    
    variant_class_name = variant_match.group(1)
    if variant_class_name != class_name:
        return None
    
    required_variants = []
    requires_pattern = r'REQUIRES\s*\(\s*(.*?)\s*\)'
    requires_match = re.search(requires_pattern, content)
    if requires_match:
        requirements = requires_match.group(1)
        required_variants = [var.strip() for var in requirements.split(',')]
    
    properties = []
    property_pattern = r'(\w+(?:::\w+)*(?:\s*\*)?)\s+(\w+)(?:\s*=\s*[^;]*)?;\s*PROPERTY\(\)(?:\s+SET_CALLBACK\((\w+)\))?'
    
    property_matches = re.finditer(property_pattern, content)
    for match in property_matches:
        prop_type = match.group(1).strip()
        prop_name = match.group(2).strip()
        has_callback = match.group(3) is not None
        callback_name = match.group(3) if has_callback else None
        
        if callback_name and callback_name != prop_name:
            print(f"Warning: Callback name '{callback_name}' doesn't match property name '{prop_name}' in {file_path}")
        
        properties.append((prop_type, prop_name, has_callback))
    
    base_class = class_match.group(3) if class_match.group(3) else "VariantBase"
    
    return {
        'class_name': class_name,
        'base_class': base_class,
        'properties': properties,
        'required_variants': required_variants
    }

def generate_raylib_registrations():
    raylib_code = """    
    rttr::registration::class_<Vector2>("Vector2")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Vector2::x)
        .property("y", &Vector2::y)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Vector3>("Vector3")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Vector3::x)
        .property("y", &Vector3::y)
        .property("z", &Vector3::z)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Rectangle>("Rectangle")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Rectangle::x)
        .property("y", &Rectangle::y)
        .property("width", &Rectangle::width)
        .property("height", &Rectangle::height)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Color>("Color")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("r", &Color::r)
        .property("g", &Color::g)
        .property("b", &Color::b)
        .property("a", &Color::a)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Camera2D>("Camera2D")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("offset", &Camera2D::offset)
        .property("target", &Camera2D::target)
        .property("rotation", &Camera2D::rotation)
        .property("zoom", &Camera2D::zoom)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Texture2D>("Texture2D")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("id", &Texture2D::id)
        .property("width", &Texture2D::width)
        .property("height", &Texture2D::height)
        .property("mipmaps", &Texture2D::mipmaps)
        .property("format", &Texture2D::format)
        (rttr::metadata("RAYLIB", true));\n\n"""

    return raylib_code

def generate_rttr_registration(classes_info):
    registration_code = "RTTR_REGISTRATION\n{\n"
    
    registration_code += """    rttr::registration::class_<VariantCreateInfo>("VariantCreateInfo")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("entity_id", &VariantCreateInfo::entity_id);
    
    rttr::registration::class_<VariantBase>("VariantBase")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("entity_id", &VariantBase::entity_id)(rttr::metadata("NO_SERIALIZE", true));\n\n"""
    
    registration_code += generate_raylib_registrations()
    
    for class_info in classes_info:
        registration_code += f'    rttr::registration::class_<{class_info["class_name"]}>("{class_info["class_name"]}")\n'
        registration_code += '        .constructor<>()(rttr::policy::ctor::as_object)\n'
        registration_code += '        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)'
        
        for prop_type, prop_name, has_callback in class_info['properties']:
            registration_code += f'\n        .property("{prop_name}", &{class_info["class_name"]}::{prop_name})'
            
            if has_callback:
                callback_method = f"on_{prop_name}_set"
                registration_code += f'(rttr::metadata("SET_CALLBACK", "{callback_method}"))'
        
        has_callbacks = any(has_callback for _, _, has_callback in class_info['properties'])
        if has_callbacks:
            registration_code += '\n'
            for prop_type, prop_name, has_callback in class_info['properties']:
                if has_callback:
                    callback_method = f"on_{prop_name}_set"
                    registration_code += f'\n        .method("{callback_method}", &{class_info["class_name"]}::{callback_method})'
        
        registration_code += ';\n\n'
    
    registration_code += "}\n"
    
    return registration_code

def generate_requires_files(classes_info, output_dir):
    for class_info in classes_info:
        if class_info['required_variants']:
            class_name = class_info['class_name']
            requires_file_path = os.path.join(output_dir, f"{class_name}.requires")
            
            requires_data = {
                "requires": class_info['required_variants']
            }
            
            with open(requires_file_path, 'w') as f:
                json.dump(requires_data, f, indent=4)
            
            print(f"Generated requirements file: {requires_file_path}")

def main(headers_dir="."):
    headers_dir = os.path.normpath(headers_dir).replace('\\', '/')
    
    header_files = glob.glob(os.path.join(headers_dir, "**/*.h"), recursive=True)
    
    classes_info = []
    includes = set()
    
    includes.add('#include "raylib.h"')
    includes.add('#include "rttr/registration.h"')
    
    for header_file in header_files:
        relative_path = os.path.normpath(header_file).replace('\\', '/')
        
        if headers_dir.startswith('include'):
            include_pattern = r'^include/'
            relative_path = re.sub(include_pattern, '', relative_path)
            
        if class_info := parse_header(header_file):
            classes_info.append(class_info)
            includes.add(f'#include "{relative_path}"')
    
    includes_code = "\n".join(sorted(includes)) + "\n\n"
    registration_code = generate_rttr_registration(classes_info)
    final_code = includes_code + registration_code
    
    output_path = os.path.join(headers_dir, "rttr_registration.h")
    with open(output_path, "w") as f:
        f.write(final_code)
    
    print(f"Generated RTTR registration code for {len(classes_info)} classes.")
    print(f"Output written to {output_path}")

    script_dir = os.path.dirname(os.path.abspath(__file__))
    requires_dir = os.path.normpath(os.path.join(script_dir, "../shared/variants/requires"))
    
    os.makedirs(requires_dir, exist_ok=True)
    generate_requires_files(classes_info, requires_dir)
    print(f"Requirements files written to {requires_dir}")

if __name__ == "__main__":
    import sys
    headers_dir = sys.argv[1] if len(sys.argv) > 1 else "."
    main(headers_dir)
