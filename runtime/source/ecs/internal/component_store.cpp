#include "ecs/internal/component_store.h"
#include <cassert>

#if DEBUG
#include <iostream>
#endif

void ComponentStore::update_component_types(const rttr::array_range<rttr::type>& types) {
    for(const auto& type : types) {
        const auto& type_name = type.get_name();

        if (!type.is_valid()) {
#if DEBUG
            std::cout << "Type is not valid" << type_name << std::endl;
#endif
            continue;
        }

        if(type.is_wrapper() || type.is_pointer()) {
#if DEBUG
            std::cout << "Skipped type: " << type_name << std::endl;
#endif
            continue;
        }

        const std::string_view& prefix = "c_";
        bool is_component =
            std::equal(prefix.begin(), prefix.end(), type_name.begin());

        if (is_component) { 
#if DEBUG
            std::cout << "Found a component type: " << type_name << std::endl;
#endif
            if(storage.find(type) == storage.end()) {
                storage[type] = component_list();
#if DEBUG
            std::cout << "Added component to storage: " << type_name << std::endl;
#endif
            }
        }
    }
}

void ComponentStore::add(Component component) {
    auto type = component.var.get_type();
    auto& list = storage[type];
    list.push_back(std::move(component));
}
 
