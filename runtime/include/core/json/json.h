#pragma once

#include <string>
#include <rttr/type>
#include <filesystem>
#include "core/entity.h"

namespace zeytin
{
    namespace json 
    {
        std::string serialize_entity(const entity_id id, const std::vector<rttr::variant>& variants);
        std::string serialize_entity(const entity_id id, const std::vector<rttr::variant>& variants, const std::filesystem::path& path);
        
        void deserialize_entity(const std::filesystem::path& path, entity_id& entity_id, std::vector<rttr::variant>& variants);
        void deserialize_entity(const std::string& entity, entity_id& entity_id, std::vector<rttr::variant>& variants);
        
        void create_dummy(const rttr::type& type);
    }
}

