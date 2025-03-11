#include <cassert>

#include "rapidjson/document.h"

#include "core/zeytin.h"
#include "core/json/json.h"
#include "core/guid/guid.h"

#include "core/variant/variant_base.h"
#include "editor/editor_event.h"

template<typename T>
void update_property(rttr::variant& obj, const std::vector<std::string>& path_parts, size_t path_index, const T& value) {
    if (path_index >= path_parts.size()) {
        return; 
    }

    const std::string& current_path = path_parts[path_index];

    if (path_index == path_parts.size() - 1) {
        // we re at the leaf property so set the value directly
        for (auto& property : obj.get_type().get_properties()) {
            if (property.get_name() == current_path) {
                std::cout << "Variant type: " << obj.get_type().get_name() << " | " << "Property type: " << property.get_type().get_name() << " | " << "Property name: " << property.get_name() << std::endl;

                property.set_value(obj, value);
                return;
            }
        }
    } else {
        // we need to navigate to a nested object
        for (auto& property : obj.get_type().get_properties()) {
            if (property.get_name() == current_path) {
                rttr::variant nested_obj = property.get_value(obj);
                update_property(nested_obj, path_parts, path_index + 1, value);
                return;
            }
        }
    }

    std::cerr << "Property " << current_path << " not found in path" << std::endl;
}

void Zeytin::generate_variants() {
    for(const auto& type : rttr::type::get_types()) {
        const auto& name = type.get_name().to_string();

        if(!type.is_derived_from<VariantBase>() || type.get_name() == "VariantBase") {
            continue;
        }
        
        try {
            create_dummy(type);
            std::cout << "Created variant for: " << name << ".variant" << std::endl;
        } catch(const std::exception& e) {
            std::cerr << "Error creating variant for " << name << ": " << e.what() << std::endl;
        }
    }    
}

void Zeytin::init() {
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

    EditorEventBus::get().subscribe<const rapidjson::Document&>(EditorEvent::EntityPropertyChanged, [this](const rapidjson::Document& doc) -> void {
        assert(!doc.HasParseError());
        assert(doc.HasMember("entity_id"));
        assert(doc.HasMember("variant_type"));
        assert(doc.HasMember("key_type"));
        assert(doc.HasMember("key_path"));
        assert(doc.HasMember("value"));

        uint64_t entity_id = doc["entity_id"].GetUint64();
        const std::string& variant_type = doc["variant_type"].GetString();
        const std::string& key_type = doc["key_type"].GetString();
        const std::string& key_path = doc["key_path"].GetString();
        const std::string& value_str = doc["value"].GetString();

        if (m_storage.find(entity_id) == m_storage.end()) {
            std::cerr << "Entity " << entity_id << " not found" << std::endl;
            return;
        }

        for (auto& variant : m_storage[entity_id]) {
            if (variant.get_type().get_name() == variant_type) {
                std::vector<std::string> path_parts;
                std::string current_part;
                std::istringstream path_stream(key_path);

                while (std::getline(path_stream, current_part, '.')) {
                    path_parts.push_back(current_part);
                }

                if (path_parts.empty()) {
                    std::cerr << "Invalid key path: " << key_path << std::endl;
                    return;
                }

                if (key_type == "int") {
                    int value = std::stoi(value_str);
                    update_property(variant, path_parts, 0, value);
                }
                else if (key_type == "float") {
                    float value = std::stof(value_str);
                    update_property(variant, path_parts, 0, value);
                }
                else if (key_type == "bool") {
                    bool value = (value_str == "true" || value_str == "1");
                    update_property(variant, path_parts, 0, value);
                }
                else if (key_type == "string") {
                    update_property(variant, path_parts, 0, value_str);
                }

                break; 
            }
        }
    });

    EditorEventBus::get().subscribe<const rapidjson::Document&>(EditorEvent::EntityVariantAdded, [this](const rapidjson::Document& msg) {
        assert(!msg.HasParseError());
        assert(msg.HasMember("entity_id"));
        assert(msg.HasMember("variant_type"));

        entity_id entity_id = msg["entity_id"].GetUint64();
        auto& variants = m_storage[entity_id];

        VariantCreateInfo info;
        info.entity_id = entity_id;
        std::vector<rttr::argument> args;
        args.push_back(info);

        rttr::type rttr_type = rttr::type::get_by_name(msg["variant_type"].GetString());
        rttr::variant obj = rttr_type.create(args);

        variants.push_back(std::move(obj));
    });

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
    entity_variants.clear();
    entity_variants.reserve(variants.size());

    for(auto& var : variants) {
        try {
            VariantBase& base = var.get_value<VariantBase&>();
            base.on_init();
            entity_variants.push_back(std::move(var));
        }
        catch(const std::exception& e) {
            std::cerr << "Exception caught: " << e.what() << std::endl;
        }
    }        
    return id;
}

entity_id Zeytin::deserialize_entity(const std::filesystem::path& path) {
    entity_id id;
    std::vector<rttr::variant> variants;

    zeytin::json::deserialize_entity(path, id, variants);

    auto& entity_variants = get_variants(id);
    entity_variants.clear();
    entity_variants.reserve(variants.size());

    for(auto& var : variants) {
        VariantBase& base = var.get_value<VariantBase&>();
        base.on_init();
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

void Zeytin::update_variants() {
    for(auto& pair : m_storage) {   
        for(auto& variant : pair.second) {
            VariantBase& base = variant.get_value<VariantBase&>();
            if(!base.is_dead) {
                base.on_update();
            }
        }
    }
}

void Zeytin::play_update_variants() {
    for(auto& pair : m_storage) {   
        for(auto& variant : pair.second) {
            VariantBase& base = variant.get_value<VariantBase&>();
            if(!base.is_dead) {
                base.on_play_update();
            }
        }
    }
}

void Zeytin::play_start_variants() {
    for(auto& pair : m_storage) {   
        for(auto& variant : pair.second) {
            VariantBase& base = variant.get_value<VariantBase&>();
            if(!base.is_dead) {
                base.on_play_start();
            }
        }
    }
}
