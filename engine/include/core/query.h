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

template<typename... Ts>
entity_id create_entity() {
    entity_id id = get_zeytin().new_entity_id();
    return id;
}

// -------------------- Component Existence Checks --------------------

template<typename T>
bool has(entity_id id) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    auto& variants = get_zeytin().get_variants(id);
    
    for (const auto& variant : variants) {
        if (variant.get_type() == rttr::type::get<T>()) {
            return true;
        }
    }
    
    return false;
}

template<typename T>
bool has(const VariantBase* base) {
    return has<T>(base->entity_id);
}

template<typename T1, typename T2, typename... Rest>
bool has(entity_id id) {
    return has<T1>(id) && has<T2, Rest...>(id);
}

template<typename T1, typename T2, typename... Rest>
bool has(const VariantBase* base) {
    return has<T1, T2, Rest...>(base->entity_id);
}

// -------------------- Component Access --------------------

template<typename T>
T& acquire(entity_id id) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    auto& variants = get_zeytin().get_variants(id);

    for (auto& variant : variants) {
        if (variant.get_type() == rttr::type::get<T>()) {
            return variant.get_value<T&>();
        }
    }

    throw std::runtime_error("Component not found despite has() check");
}

template<typename T>
T& acquire(const VariantBase* base) {
    return acquire<T>(base->entity_id);
}

template<typename T1, typename T2, typename... Rest>
std::tuple<T1&, T2&, Rest&...> acquire(entity_id id) {
    return std::tie(acquire<T1>(id), acquire<T2>(id), acquire<Rest>(id)...);
}

template<typename T1, typename T2, typename... Rest>
std::tuple<T1&, T2&, Rest&...> acquire(const VariantBase* base) {
    return acquire<T1, T2, Rest...>(base->entity_id);
}

template<typename T>
std::optional<std::reference_wrapper<T>> try_acquire(entity_id id) {
    if (has<T>(id)) {
        return std::optional<std::reference_wrapper<T>>(std::ref(acquire<T>(id)));
    }
    return std::nullopt;
}

template<typename T>
std::optional<std::reference_wrapper<T>> try_acquire(const VariantBase* base) {
    return try_acquire<T>(base->entity_id);
}

// -------------------- Read-Only Component Access --------------------

template<typename T>
const T& read(entity_id id) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    return acquire<T>(id);
}

template<typename T>
const T& read(const VariantBase* base) {
    return read<T>(base->entity_id);
}

template<typename T1, typename T2, typename... Rest>
std::tuple<const T1&, const T2&, const Rest&...> read(entity_id id) {
    return std::tie(read<T1>(id), read<T2>(id), read<Rest>(id)...);
}

template<typename T1, typename T2, typename... Rest>
std::tuple<const T1&, const T2&, const Rest&...> read(const VariantBase* base) {
    return read<T1, T2, Rest...>(base->entity_id);
}

// -------------------- Entity/Component Finding --------------------

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
std::vector<std::reference_wrapper<T>> find_all() {
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
std::vector<entity_id> find_all_with() {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    std::vector<entity_id> results;
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        if (has<T, Rest...>(entity_id)) {
            results.push_back(entity_id);
        }
    }
    
    return results;
}

template<typename T>
std::vector<std::reference_wrapper<T>> find_where(std::function<bool(T&)> predicate) {
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

// -------------------- Helper Functions --------------------

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

// -------------------- Component Removal --------------------

template<typename T>
void remove_variant_from(entity_id id) {
    get_zeytin().remove_variant(id, rttr::type::get<T>());
}

template<typename T>
void remove_variant_from(const VariantBase* base) {
    get_zeytin().remove_variant(base->entity_id, rttr::type::get<T>());
}

} // namespace Query
