#pragma once

#include <vector>
#include <unordered_map>

#include "rttr/type.h"
#include "rttr/array_range.h"

#include "ecs/internal/component.h"

#include <cassert>

using component_list = std::vector<Component>;

class ComponentStore {

public:
    void update_component_types(const rttr::array_range<rttr::type>& types);
    void add(Component component);

    template<class T>
    component_list& get_component_list_unsafe() {
        auto it = storage.find(rttr::type::get<T>());
        assert(it != storage.end());
        return it->second;
    }

private:
    std::unordered_map<rttr::type, component_list> storage;
};
