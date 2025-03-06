#pragma once

#include "core/entity.h"
#include "rttr/registration.h"

struct VariantBase final {
    VariantBase(entity_id id) : id(id) {}

    entity_id id;

    RTTR_ENABLE();
};
