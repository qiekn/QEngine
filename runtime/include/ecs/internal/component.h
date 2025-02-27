#pragma once

#include "rttr/variant.h"

struct Component final {
    rttr::variant var;
    
    template<typename T>
    const T& get() const {
        return var.get_value<T>();
    }

    template<typename T>
    T& get() {
        if(var.get_type().is_pointer()) {
            return *var.get_value<T*>();
        }

        return var.get_value<T>();
    }

    const rttr::variant& get_variant() {
        return var;
    }

    template<typename T, typename... Args>
    static Component from_type(Args&&... args) {
        rttr::variant var(std::move(T(std::forward<Args>(args)...)));
        return {
            .var = var
        };
    }

};
