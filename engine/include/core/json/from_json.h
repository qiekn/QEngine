#pragma once

#include <rttr/argument.h>
#include <rttr/type.h>
#include <vector>
#include "entity/entity.h"
#include "rttr/variant.h"

namespace rttr_json {
entity_id deserialize_entity(const std::string& entity_json, entity_id& entity,
                             std::vector<rttr::variant>& variants);
}
