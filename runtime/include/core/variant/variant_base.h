#pragma once

#include "rttr/rttr_enable.h"
#include "core/entity.h"
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

    template<typename T>
    T& get_variant_or_default() {
        T default_instance{};
        auto variant_ref = Zeytin::get().try_get_variant<T>(entity_id);
        return variant_ref.has_value() ? variant_ref.value().get() : default_instance;
    }

    template<typename T>
    VariantRef<T> get_variant() {
        return Zeytin::get().try_get_variant<T>(entity_id);
    }

    template<typename T>
    VariantRef<T> entity_get_variant(entity_id id) {
        return Zeytin::get().try_get_variant<T>(id);
    }

    entity_id entity_id;
    bool is_dead = false;

    RTTR_ENABLE();
};
