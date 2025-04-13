#pragma once

#include <vector>
#include <functional>
#include <type_traits>
#include <optional>

#include "core/zeytin.h"
#include "rttr/variant.h"
#include "entity/entity.h"
#include "variant/variant_base.h"

namespace Query {

template<typename T>
std::optional<std::reference_wrapper<T>> find_first() {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    const rttr::type& type = rttr::type::get<T>();
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        for (auto& variant : variants) {
            if (variant.get_type() == type) {
                return std::ref(variant.get_value<T&>());
            }
        }
    }
    
    return std::nullopt;
}

template<typename T>
std::vector<T&> find_all() {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    std::vector<std::reference_wrapper<T>> results;
    const rttr::type& type = rttr::type::get<T>();
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        for (auto& variant : variants) {
            if (variant.get_type() == type) {
                T& component = variant.get_value<T&>();
                results.push_back(std::ref(component));
            }
        }
    }
    
    return results;
}

template<typename T, typename... Rest>
bool has_types(const std::vector<rttr::variant>& variants) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    bool has_t = false;
    for (const auto& variant : variants) {
        if (variant.get_type() == rttr::type::get<T>()) {
            has_t = true;
            break;
        }
    }
    
    if (!has_t) {
        return false;
    }
    
    if constexpr (sizeof...(Rest) > 0) {
        return has_types<Rest...>(variants);
    }
    
    return true;
}

template<typename T, typename... Rest>
std::vector<entity_id> find_all_with() {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    std::vector<entity_id> results;
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        bool has_all = true;
        
        bool has_t = false;
        for (auto& variant : variants) {
            if (variant.get_type() == rttr::type::get<T>()) {
                has_t = true;
                break;
            }
        }
        
        if (!has_t) {
            has_all = false;
        }
        
        if constexpr (sizeof...(Rest) > 0) {
            has_all = has_all && has_types<Rest...>(variants);
        }
        
        if (has_all) {
            results.push_back(entity_id);
        }
    }
    
    return results;
}

template<typename T>
std::vector<T&> find_where(std::function<bool(T&)> predicate) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    std::vector<std::reference_wrapper<T>> results;
    const rttr::type& type = rttr::type::get<T>();
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        for (auto& variant : variants) {
            if (variant.get_type() == type) {
                T& component = variant.get_value<T&>();
                if (predicate(component)) {
                    results.push_back(std::ref(component));
                }
            }
        }
    }
    
    return results;
}

template<typename T>
size_t count() {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    size_t count = 0;
    const rttr::type& type = rttr::type::get<T>();
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        for (auto& variant : variants) {
            if (variant.get_type() == type) {
                count++;
                break;
            }
        }
    }
    
    return count;
}


template<typename T>
void for_each(std::function<void(T&)> action) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    const rttr::type& type = rttr::type::get<T>();
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        for (auto& variant : variants) {
            if (variant.get_type() == type) {
                T& component = variant.get_value<T&>();
                action(component);
            }
        }
    }
}

template<typename T>
void remove_variant_from(entity_id id) {
    get_zeytin().remove_variant(id, rttr::type::get<T>());
}

template<typename T>
void remove_variant_from(const VariantBase* base) {
    get_zeytin().remove_variant(base->entity_id, rttr::type::get<T>());
}

template<typename T>
T& acquire(entity_id id) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    auto& variants = get_zeytin().get_variants(id);

    for (auto& variant : variants) {
        if (variant.get_type() == rttr::type::get<T>()) {
            return variant.get_value<T&>();
        }
    }

    throw std::runtime_error("Component not found despite waits() check");
}

template<typename T>
T& acquire(const VariantBase* base) {
    return acquire<T>(base->entity_id);
}

template<typename... Ts>
std::tuple<Ts&...> acquire_all(entity_id id) {
    return std::tie(acquire<Ts>(id)...);
}

template<typename... Ts>
std::tuple<Ts&...> acquire_all(const VariantBase* base) {
    return acquire_all<Ts...>(base->entity_id);
}

template<typename... Ts>
bool has(const VariantBase* base) {
    return has<Ts...>(base->entity_id);
}

template<typename... Ts>
bool has(entity_id id) {
    static_assert((std::is_base_of<VariantBase, Ts>::value && ...),
                  "All types must derive from VariantBase");

    return (has_types<Ts>(id) && ...);
}
} 



