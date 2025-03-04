#pragma once

#include "entity/entity_document.h"
#include "variant/variant_document.h"

#include <vector>
#include <unordered_map>

class Hierarchy final {
public: 
    inline Hierarchy(std::vector<EntityDocument>& entities, std::vector<VariantDocument>& variants) 
        : m_entities(entities), m_variants(variants) {}

    void update();

private:
    std::vector<EntityDocument>& m_entities;
    std::vector<VariantDocument>& m_variants;

    void render_create_entity();
    void render_entity(EntityDocument& entity);
    void render_variant(rapidjson::Document& document, rapidjson::Value& variant, int index); // index is required for unique id
    void render_object(rapidjson::Document& document, rapidjson::Value& object);
    void add_variant_to_entity(EntityDocument& entity_document, VariantDocument& variant_document);
};
