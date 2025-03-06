#include <cassert>

#include "core/zeytin.h"
#include "core/json/json.h"
#include "core/guid/guid.h"


void Zeytin::init() {
    for(const auto& type : rttr::type::get_types()) {
        const auto& name = type.get_name().to_string();
        
        if(!type.is_class() || 
           type.is_wrapper() || 
           type.is_pointer() || 
           type.is_template_instantiation() || 
           name == "VariantBase" || 
           name == "VariantCreateInfo") {
            continue;      
        }
        
        try {
            create_dummy(type);
            std::cout << "Created variant for: " << name << ".variant" << std::endl;
        } catch(const std::exception& e) {
            std::cerr << "Error creating variant for " << name << ": " << e.what() << std::endl;
        }
    }    

    std::cout << "Parsing entities from path: " << std::filesystem::absolute("../shared/entities") << std::endl;

    try {
        int file_count = 0;
        int entity_count = 0;

        for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("../shared/entities")) {
            file_count++;
            std::cout << "Found file: " << entry.path().filename() << std::endl;

            if(!entry.is_regular_file() || entry.path().extension() != ".entity") {
                std::cout << "  Skipping (not a .entity file)" << std::endl;
                continue;
            }

            std::filesystem::path file_path = entry.path();
            std::string file_name = file_path.stem().string();
            std::cout << "  Parsing entity: " << file_name << std::endl;

            deserialize_entity(file_path);
            entity_count++;
        }

        std::cout << "Parsed " << entity_count << " entities out of " << file_count << std::endl;
    } catch(const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

void Zeytin::add_variant(const entity_id& entity, rttr::variant variant) {
    m_storage[entity].push_back(std::move(variant));
}

std::string Zeytin::serialize_entity(const entity_id id) {
    return zeytin::json::serialize_entity(id, get_variants(id));
}

std::string Zeytin::serialize_entity(const entity_id id, const std::filesystem::path& path) {
    return zeytin::json::serialize_entity(id, get_variants(id), path);
}

entity_id Zeytin::deserialize_entity(const std::string& str) {
    std::cout << "Deserializng entity: " << str << std::endl;

    entity_id id;
    std::vector<rttr::variant> variants;

    zeytin::json::deserialize_entity(str, id, variants);

    auto& entity_variants = get_variants(id);
    for(const auto& var : variants) {
        entity_variants.push_back(std::move(var));
    }        
    return id;
}


entity_id Zeytin::deserialize_entity(const std::filesystem::path& path) {
    entity_id id;
    std::vector<rttr::variant> variants;

    zeytin::json::deserialize_entity(path, id, variants);

    auto& entity_variants = get_variants(id);
    for(const auto& var : variants) {
        entity_variants.push_back(std::move(var));
    }
    return id;
}

void Zeytin::create_dummy(const rttr::type& type) {
    zeytin::json::create_dummy(type);
}

entity_id Zeytin::new_entity_id() {
    return generateUniqueID();
}
