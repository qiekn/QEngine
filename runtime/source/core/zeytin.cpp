#include <cassert>

#include "core/zeytin.h"
#include "core/json/json.h"
#include "core/guid/guid.h"

Zeytin::Zeytin() {
    const auto& types = rttr::type::get_types();
    for(const auto& type : types) {
        if(type.is_wrapper() || type.is_pointer() || !type.is_class() || type.is_template_instantiation()) {
            continue;
        }
        create_dummy(type);
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

void Zeytin::deserialize_entity(const std::string& str) {
    // TODO: implement a system where we read entity id by file name instead of parsing it which removes copying of variants

    entity_id id;
    std::vector<rttr::variant> variants;

    zeytin::json::deserialize_entity(str, id, variants);

    auto& entity_variants = get_variants(id);
    for(const auto& var : variants) {
        entity_variants.push_back(std::move(var));
    }        
}


void Zeytin::deserialize_entity(const std::filesystem::path& path) {
    // TODO: implement a system where we read entity id by file name instead of parsing it which removes copying of variants

    entity_id id;
    std::vector<rttr::variant> variants;

    zeytin::json::deserialize_entity(path, id, variants);

    auto& entity_variants = get_variants(id);
    for(const auto& var : variants) {
        entity_variants.push_back(std::move(var));
    }
}

void Zeytin::create_dummy(const rttr::type& type) {
    zeytin::json::create_dummy(type);
}

entity_id Zeytin::new_entity_id() {
    return generateUniqueID();
}
