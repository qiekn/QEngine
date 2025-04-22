#pragma once

#include <string>
#include "entity/entity.h"
#include <vector>
#include <rttr/variant.h>
#include <filesystem>

namespace rttr_json {
    std::string serialize_entity(const entity_id entity_id, const std::vector<rttr::variant>& variants);
    std::string serialize_entity(const entity_id entity_id, const std::vector<rttr::variant>& variants, const std::filesystem::path& path);
    void create_dummy(const rttr::type& type);
}
