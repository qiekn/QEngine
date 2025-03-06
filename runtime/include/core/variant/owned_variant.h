 #pragma once

#include "rttr/variant.h"
#include "core/entity.h"

struct OwnedVariant {
    OwnedVariant(entity_id id, rttr::variant var) : entity_id(id), variant(var) {}

    rttr::variant variant;
    entity_id entity_id;
};
