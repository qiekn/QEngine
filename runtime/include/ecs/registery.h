#pragma once

#include "ecs/internal/component_store.h"
#include "ecs/internal/component.h"
#include "ecs/entity.h"

#include <iostream>
#include <cassert>

class Registery {

public:
    Registery();

    template<typename T, typename... Args>
        bool add_component(Entity entity, Args&&... args) {
        T t(std::forward<Args>(args)...);

        rttr::variant var(std::move(t));
        assert(var.is_valid());


        Component component = {
            .var = var,
            .entity = entity
        };

        component_store.add(std::move(component));

        return true;
    }   

    template<typename T>
    component_list& query() {
        return component_store.get_component_list_unsafe<T>();
    }

private:
    ComponentStore component_store;
};
