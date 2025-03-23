#pragma once

#include "rttr/rttr_enable.h"
#include "core/zeytin.h"

#define VARIANT(ClassName) public: ClassName() = default; ClassName(VariantCreateInfo info) : VariantBase(info) {} RTTR_ENABLE(VariantBase); private:
#define PROPERTY() 

#define SET_CALLBACK(property_name) \
    void on_##property_name##_set();

template<typename T>
using VariantRef = std::optional<std::reference_wrapper<T>>;

struct VariantCreateInfo {
    entity_id entity_id;

    RTTR_ENABLE();
};

struct VariantBase {
    VariantBase() = default;
    VariantBase(VariantCreateInfo info) : entity_id(info.entity_id) {}

    virtual void on_init() {}
    virtual void on_post_init() {}
    virtual void on_update() {}
    virtual void on_play_start() {}
    virtual void on_play_update() {}

    uint64_t get_id() { return entity_id; }
    const uint64_t get_id() const { return entity_id; }

    entity_id entity_id;
    bool is_dead = false;

    RTTR_ENABLE();
};
