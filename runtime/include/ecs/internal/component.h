#pragma once

#include "rttr/variant.h"
#include "entity.h"

struct Component final {
    rttr::variant var;
    Entity entity;
    
    template<typename T>
    const T& get() const {
        return var.get_value<T>();
    }

    template<typename T>
    T& get() {
        return var.get_value<T>();
    }

};
