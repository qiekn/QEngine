#pragma once

#include "rttr/rttr_enable.h"
#include "core/entity.h"
#include "core/zeytin.h"


struct IVariantBase {
    IVariantBase(entity_id id) : id(id) {}

    entity_id id;

    RTTR_ENABLE();
};
