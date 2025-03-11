#pragma once

#include "rttr/rttr_enable.h"
#include "core/entity.h"
#include "core/zeytin.h"

#define PROPERTY() // does not do anything. just to hint

struct VariantCreateInfo {
    entity_id entity_id;

    RTTR_ENABLE();
};

struct VariantBase {
    VariantBase() = default;
    VariantBase(VariantCreateInfo info) : entity_id(info.entity_id) {}

    virtual void awake() {}
    virtual void tick() {}

    uint64_t get_id() { return entity_id; }
    const uint64_t get_id() const { return entity_id; }

    template<typename T>
    void entity_get_variant(rttr::variant& out_variant) {
        return Zeytin::get().try_get_variant<T>(entity_id, out_variant);
    }

    template<typename T>
    void entity_get_variant(entity_id id, rttr::variant& out_variant) {
        Zeytin::get().try_get_variant<T>(id, out_variant);
    }

    entity_id entity_id;
    bool is_dead = false;

    RTTR_ENABLE();
};
