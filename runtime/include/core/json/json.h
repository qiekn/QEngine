#pragma once

#include <string>
#include <rttr/type>
#include <filesystem>
#include "core/internal/entity.h"

namespace json
{
    std::string to(rttr::instance obj, const std::string& path);
    rttr::variant from(const std::string& json_as_string);
    rttr::variant from(const std::filesystem::path& path_to_json);
    void create_dummy(const rttr::type& type);
    // TODO: remove above


    std::string to(const entity_id& entity);
    void from_entity(const std::filesystem::path& path_to_entity);
}

