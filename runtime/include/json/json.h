#pragma once

#include <string>
#include <rttr/type>
#include <filesystem>

namespace json
{
    std::string to(rttr::instance obj, const std::string& path);
    bool from(const std::string& json, rttr::instance obj);
    rttr::type get_type(const std::string& json);
}

