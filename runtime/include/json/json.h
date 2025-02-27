#pragma once

#include <string>
#include <rttr/type>
#include <filesystem>

namespace json
{
    std::string to(rttr::instance obj, const std::string& path);
    rttr::variant from(const std::string& json_as_string);
    rttr::variant from(const std::filesystem::path& path_to_json);
    void create_dummy(const rttr::type& type);
}

