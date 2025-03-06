#pragma once

#include "rttr/rttr_enable.h"
#include "core/entity.h"
#include "core/zeytin.h"

struct VariantCreateInfo {
    entity_id entity_id;

    RTTR_ENABLE();
};

struct VariantBase {
    VariantBase() = default;
    VariantBase(VariantCreateInfo info) : entity_id(info.entity_id) {}

    uint64_t get_id() { return entity_id; }
    const uint64_t get_id() const { return entity_id; }

    template<typename T>
    T& entity_get_variant() {
        return Zeytin::get().get_first<T>(entity_id);
    }

    template<typename T>
    T& entity_get_variant(entity_id id) {
        return Zeytin::get().get_first<T>(id);
    }

    entity_id entity_id;

    RTTR_ENABLE();
};
