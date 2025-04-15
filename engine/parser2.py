#!/usr/bin/env python3

import os
import re
import glob
import json
import argparse
from pathlib import Path
from typing import Dict, List, Tuple, Optional, Set, Any, Union


class ClassParser:
    def __init__(self):
        self.variant_pattern = re.compile(r'VARIANT\((\w+)\)')
        self.class_pattern = re.compile(r'(struct|class)\s+(\w+)\s*(?::\s*public\s+(\w+))?')
        self.requires_pattern = re.compile(r'REQUIRES\s*\(\s*(.*?)\s*\)')
        self.property_pattern = re.compile(r'(\w+(?:::\w+)*(?:\s*\*)?)\s+(\w+)(?:\s*=\s*[^;]*)?;\s*PROPERTY\(\)(?:\s+SET_CALLBACK\((\w+)\))?')
        
        self.regular_property_pattern = re.compile(r'^\s*(?:public|private|protected)?:?\s*(\w+(?:::\w+)*(?:\s*\*)?)\s+(\w+)\s*;')
        
        self.query_get_pattern = re.compile(r'Query::get<([\w,\s]+)>\(this\)')
        self.query_read_pattern = re.compile(r'Query::read<([\w,\s]+)>\(this\)')
        self.query_try_get_pattern = re.compile(r'Query::try_get<([\w,\s]+)>\(this\)')
        
        self.skip_classes = ["VariantCreateInfo", "VariantBase"]

    def clean_content(self, content: str) -> str:
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
        
        return content

    def find_class_block(self, content: str, class_match: re.Match) -> str:
        class_start = class_match.start()
        class_block = content[class_start:]
        
        open_braces = 0
        close_pos = 0
        
        for i, char in enumerate(class_block):
            if char == '{':
                open_braces += 1
            elif char == '}':
                open_braces -= 1
                if open_braces == 0:
                    close_pos = i + 1  
                    break
        
        if close_pos > 0:
            return class_block[:close_pos]
        return class_block

    def parse_variant_class(self, class_block: str, class_name: str, base_class: str) -> Optional[Dict[str, Any]]:
        if class_name in self.skip_classes:
            return None
        
        required_variants = []
        requires_match = self.requires_pattern.search(class_block)
        if requires_match:
            requirements = requires_match.group(1)
            required_variants = [var.strip() for var in requirements.split(',')]
        
        properties = []
        property_matches = self.property_pattern.finditer(class_block)
        for match in property_matches:
            prop_type = match.group(1).strip()
            prop_name = match.group(2).strip()
            has_callback = match.group(3) is not None
            callback_name = match.group(3) if has_callback else None
            
            properties.append((prop_type, prop_name, has_callback))
        
        base_class = base_class if base_class else "VariantBase"
        
        return {
            'class_name': class_name,
            'base_class': base_class,
            'properties': properties,
            'required_variants': required_variants,
            'is_variant': True
        }

    def parse_regular_class(self, class_block: str, class_name: str, base_class: str) -> Optional[Dict[str, Any]]:
        properties = []
        
        for line in class_block.split('\n'):
            property_match = self.property_pattern.search(line)
            if property_match:
                prop_type = property_match.group(1).strip()
                prop_name = property_match.group(2).strip()
                has_callback = property_match.group(3) is not None
                
                properties.append((prop_type, prop_name, has_callback))
                continue
            
            regular_match = self.regular_property_pattern.search(line)
            if regular_match:
                prop_type = regular_match.group(1).strip()
                prop_name = regular_match.group(2).strip()
                
                if '(' in line or 'operator' in line:
                    continue
                
                properties.append((prop_type, prop_name, False))
        
        if not properties:
            return None
        
        return {
            'class_name': class_name,
            'base_class': base_class,
            'properties': properties,
            'required_variants': [],
            'is_variant': False
        }

    def parse_header(self, file_path: str) -> List[Dict[str, Any]]:
        try:
            with open(file_path, 'r') as f:
                content = f.read()
        except Exception as e:
            print(f"Error reading file {file_path}: {e}")
            return []
        
        content = self.clean_content(content)
        
        class_matches = list(self.class_pattern.finditer(content))
        
        variant_matches = list(self.variant_pattern.finditer(content))
        
        variant_classes = {}
        for variant_match in variant_matches:
            variant_class_name = variant_match.group(1)
            variant_classes[variant_class_name] = True
        
        results = []
        
        for class_match in class_matches:
            class_type = class_match.group(1)  
            class_name = class_match.group(2)
            base_class = class_match.group(3)
            
            if '<' in class_name or '>' in class_name:
                continue
            
            class_block = self.find_class_block(content, class_match)
            
            is_variant = class_name in variant_classes
            
            if is_variant:
                variant_info = self.parse_variant_class(class_block, class_name, base_class)
                if variant_info:
                    results.append(variant_info)
            else:
                regular_info = self.parse_regular_class(class_block, class_name, base_class)
                if regular_info:
                    results.append(regular_info)
        
        return results
    
    def find_cpp_file(self, header_path: str, source_root: str) -> Optional[str]:
        header_basename = os.path.basename(os.path.splitext(header_path)[0])
        cpp_filename = header_basename + ".cpp"
        
        header_dir_parts = os.path.dirname(header_path).split(os.sep)
        category_dir = None
        for part in header_dir_parts:
            if part != "include" and part != "engine" and part != "editor":
                category_dir = part
                break
        
        potential_paths = []
        
        if category_dir:
            potential_paths.append(os.path.join(source_root, category_dir, cpp_filename))
        
        potential_paths.append(os.path.join(source_root, cpp_filename))
        
        for path in potential_paths:
            if os.path.exists(path):
                return path
        
        for root, _, files in os.walk(source_root):
            if cpp_filename in files:
                return os.path.join(root, cpp_filename)
        
        return None
    
    def extract_query_dependencies(self, cpp_content: str) -> Set[str]:
        dependencies = set()
        
        cpp_content = self.clean_content(cpp_content)
        
        for match in self.query_get_pattern.finditer(cpp_content):
            template_params = match.group(1)
            variants = [param.strip() for param in template_params.split(',')]
            dependencies.update(variants)
        
        for match in self.query_read_pattern.finditer(cpp_content):
            template_params = match.group(1)
            variants = [param.strip() for param in template_params.split(',')]
            dependencies.update(variants)
        
        for match in self.query_try_get_pattern.finditer(cpp_content):
            template_params = match.group(1)
            variants = [param.strip() for param in template_params.split(',')]
            dependencies.update(variants)
        
        return dependencies


class CodeGenerator:
    @staticmethod
    def generate_base_classes_registration() -> str:
        return """    rttr::registration::class_<VariantCreateInfo>("VariantCreateInfo")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("entity_id", &VariantCreateInfo::entity_id);
    
    rttr::registration::class_<VariantBase>("VariantBase")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("entity_id", &VariantBase::entity_id)(rttr::metadata("NO_SERIALIZE", true));\n\n"""

    @staticmethod
    def generate_raylib_registration() -> str:
        return """    rttr::registration::class_<Vector2>("Vector2")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Vector2::x)
        .property("y", &Vector2::y)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Vector3>("Vector3")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Vector3::x)
        .property("y", &Vector3::y)
        .property("z", &Vector3::z)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Rectangle>("Rectangle")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Rectangle::x)
        .property("y", &Rectangle::y)
        .property("width", &Rectangle::width)
        .property("height", &Rectangle::height)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Color>("Color")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("r", &Color::r)
        .property("g", &Color::g)
        .property("b", &Color::b)
        .property("a", &Color::a)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Camera2D>("Camera2D")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("offset", &Camera2D::offset)
        .property("target", &Camera2D::target)
        .property("rotation", &Camera2D::rotation)
        .property("zoom", &Camera2D::zoom)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Texture2D>("Texture2D")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("id", &Texture2D::id)
        .property("width", &Texture2D::width)
        .property("height", &Texture2D::height)
        .property("mipmaps", &Texture2D::mipmaps)
        .property("format", &Texture2D::format)
        (rttr::metadata("NO_VARIANT", true));\n\n"""

    @staticmethod
    def generate_variant_class_registration(class_info: Dict[str, Any]) -> str:
        class_name = class_info["class_name"]
        
        code = f'    rttr::registration::class_<{class_name}>("{class_name}")\n'
        code += '        .constructor<>()(rttr::policy::ctor::as_object)\n'
        code += '        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)'
        
        for prop_type, prop_name, has_callback in class_info['properties']:
            code += f'\n        .property("{prop_name}", &{class_name}::{prop_name})'
            
            if has_callback:
                callback_method = f"on_{prop_name}_set"
                code += f'(rttr::metadata("SET_CALLBACK", "{callback_method}"))'
        
        has_callbacks = any(has_callback for _, _, has_callback in class_info['properties'])
        if has_callbacks:
            code += '\n'
            for prop_type, prop_name, has_callback in class_info['properties']:
                if has_callback:
                    callback_method = f"on_{prop_name}_set"
                    code += f'\n        .method("{callback_method}", &{class_name}::{callback_method})'
        
        code += ';\n\n'
        return code

    @staticmethod
    def generate_regular_class_registration(class_info: Dict[str, Any]) -> str:
        class_name = class_info["class_name"]
        
        code = f'    rttr::registration::class_<{class_name}>("{class_name}")\n'
        code += '        .constructor<>()(rttr::policy::ctor::as_object)'
        
        for prop_type, prop_name, _ in class_info['properties']:
            code += f'\n        .property("{prop_name}", &{class_name}::{prop_name})'
        
        code += '\n        (rttr::metadata("NO_VARIANT", true))'
        
        code += ';\n\n'
        return code

    @staticmethod
    def generate_full_registration(classes_info: List[Dict[str, Any]]) -> str:
        registration_code = "RTTR_REGISTRATION\n{\n"
        
        registration_code += CodeGenerator.generate_base_classes_registration()
        
        registration_code += CodeGenerator.generate_raylib_registration()
        
        for class_info in classes_info:
            if class_info['is_variant']:
                registration_code += CodeGenerator.generate_variant_class_registration(class_info)
            else:
                registration_code += CodeGenerator.generate_regular_class_registration(class_info)
        
        registration_code += "}\n"
        return registration_code

    @staticmethod
    def generate_requires_file(class_name: str, required_variants: List[str], output_dir: str) -> None:
        requires_file_path = os.path.join(output_dir, f"{class_name}.requires")
        
        requires_data = {
            "requires": required_variants
        }
        
        try:
            with open(requires_file_path, 'w') as f:
                json.dump(requires_data, f, indent=4)
            print(f"Generated requirements file: {requires_file_path}")
        except Exception as e:
            print(f"Error writing requirements file {requires_file_path}: {e}")


class RTTRGenerator:
    def __init__(self, headers_dir: str):
        self.headers_dir = os.path.normpath(headers_dir).replace('\\', '/')
        self.parser = ClassParser()
        self.classes_info = []
        self.includes = set()
        
        self.includes.add('#include "raylib.h"')
        self.includes.add('#include "rttr/registration.h"')

    def process_headers(self) -> None:
        header_files = glob.glob(os.path.join(self.headers_dir, "**/*.h"), recursive=True)
        
        for header_file in header_files:
            relative_path = os.path.normpath(header_file).replace('\\', '/')
            
            if self.headers_dir.startswith('include'):
                include_pattern = r'^include/'
                relative_path = re.sub(include_pattern, '', relative_path)
            
            class_infos = self.parser.parse_header(header_file)
            if class_infos:
                for class_info in class_infos:
                    self.classes_info.append(class_info)
                    self.includes.add(f'#include "{relative_path}"')

    def analyze_implementation_files(self, source_root: str) -> None:
        for class_info in self.classes_info:
            if not class_info['is_variant']:
                continue
                
            class_name = class_info['class_name'].lower()
            
            for header_file in glob.glob(os.path.join(self.headers_dir, "**/*.h"), recursive=True):
                if os.path.basename(header_file) == f"{class_name}.h":
                    cpp_file = self.parser.find_cpp_file(header_file, source_root)
                    if cpp_file:
                        try:
                            with open(cpp_file, 'r') as f:
                                cpp_content = f.read()
                            
                            dependencies = self.parser.extract_query_dependencies(cpp_content)
                            
                            for dep in dependencies:
                                if dep not in class_info['required_variants'] and dep != class_name:
                                    class_info['required_variants'].append(dep)
                                    print(f"Added auto-detected dependency: {class_name} requires {dep}")
                        except Exception as e:
                            print(f"Error analyzing cpp file {cpp_file}: {e}")
                    break

    def generate_rttr_header(self, output_path: str) -> None:
        includes_code = "\n".join(sorted(self.includes)) + "\n\n"
        registration_code = CodeGenerator.generate_full_registration(self.classes_info)
        final_code = includes_code + registration_code
        
        try:
            with open(output_path, "w") as f:
                f.write(final_code)
            
            variant_count = sum(1 for c in self.classes_info if c['is_variant'])
            regular_count = sum(1 for c in self.classes_info if not c['is_variant'])
            
            print(f"Generated RTTR registration code for {len(self.classes_info)} classes:")
            print(f"  - {variant_count} variant classes")
            print(f"  - {regular_count} regular classes")
            print(f"Output written to {output_path}")
        except Exception as e:
            print(f"Error writing RTTR header {output_path}: {e}")

    def generate_requires_files(self, requires_dir: str) -> None:
        os.makedirs(requires_dir, exist_ok=True)
        
        for class_info in self.classes_info:
            if class_info['is_variant'] and class_info['required_variants']:
                CodeGenerator.generate_requires_file(
                    class_info['class_name'], 
                    class_info['required_variants'], 
                    requires_dir
                )
        
        print(f"Requirements files written to {requires_dir}")

    def run(self, source_root: str) -> None:
        self.process_headers()
        self.analyze_implementation_files(source_root)
        
        output_path = os.path.join(self.headers_dir, "rttr_registration.h")
        self.generate_rttr_header(output_path)
        
        script_dir = os.path.dirname(os.path.abspath(__file__))
        requires_dir = os.path.normpath(os.path.join(script_dir, "../shared_resources/variants/requires"))
        self.generate_requires_files(requires_dir)


def main():
    parser = argparse.ArgumentParser(description='Generate RTTR registration code for Zeytin engine.')
    parser.add_argument('headers_dir', nargs='?', default=".", 
                        help='Directory containing header files to parse')
    parser.add_argument('--source-root', default="source", 
                        help='Root directory containing implementation files')
    
    args = parser.parse_args()
    
    generator = RTTRGenerator(args.headers_dir)
    generator.run(args.source_root)


if __name__ == "__main__":
    main()
