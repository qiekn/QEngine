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
std::optional<T&> find_first() {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    const rttr::type& type = rttr::type::get<T>();
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        for (auto& variant : variants) {
            if (variant.get_type() == type) {
                T& component = variant.get_value<T&>();
                return component;
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
bool check_has_types(const std::vector<rttr::variant>& variants) {
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
        return check_has_types<Rest...>(variants);
    }
    
    return true;
}

template<typename T, typename... Rest>
std::vector<entity_id> find_entities_with_all() {
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
            has_all = has_all && check_has_types<Rest...>(variants);
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
std::optional<std::reference_wrapper<T>> get_component(entity_id id) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    auto& variants = get_zeytin().get_variants(id);
    
    for (auto& variant : variants) {
        if (variant.get_type() == rttr::type::get<T>()) {
            return std::ref(variant.get_value<T&>());
        }
    }
    
    return std::nullopt;
}

template<typename T, typename... Rest>
std::optional<std::tuple<std::reference_wrapper<T>, std::reference_wrapper<Rest>...>>
get_rest_components(entity_id id) {
    auto t_opt = get_component<T>(id);
    
    if (!t_opt) {
        return std::nullopt;
    }
    
    if constexpr (sizeof...(Rest) > 0) {
        auto rest_opt = get_rest_components<Rest...>(id);
        if (!rest_opt) {
            return std::nullopt;
        }
        
        return std::tuple_cat(
            std::make_tuple(t_opt.value()),
            rest_opt.value()
        );
    } else {
        return std::make_tuple(t_opt.value());
    }
}

template<typename T>
std::optional<std::reference_wrapper<T>> get_component(const VariantBase* base) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    return get_component<T>(base->entity_id);
}

template<typename T, typename U, typename... Rest>
std::optional<std::tuple<std::reference_wrapper<T>, std::reference_wrapper<U>, std::reference_wrapper<Rest>...>> 
get_components(entity_id id) {
    static_assert(std::is_base_of<VariantBase, T>::value, "T must derive from VariantBase");
    static_assert(std::is_base_of<VariantBase, U>::value, "U must derive from VariantBase");
    static_assert((... && std::is_base_of<VariantBase, Rest>::value), "All types must derive from VariantBase");
    
    auto t_opt = get_component<T>(id);
    auto u_opt = get_component<U>(id);
    
    if (!t_opt || !u_opt) {
        return std::nullopt;
    }
    
    if constexpr (sizeof...(Rest) > 0) {
        auto rest_opt = get_rest_components<Rest...>(id);
        if (!rest_opt) {
            return std::nullopt;
        }
        
        return std::tuple_cat(
            std::make_tuple(t_opt.value(), u_opt.value()),
            rest_opt.value()
        );
    } else {
        return std::make_tuple(t_opt.value(), u_opt.value());
    }
}

template<typename T, typename U, typename... Rest>
std::optional<std::tuple<std::reference_wrapper<T>, std::reference_wrapper<U>, std::reference_wrapper<Rest>...>> 
get_components(const VariantBase* base) {
    return get_components<T, U, Rest...>(base->entity_id);
}

template<typename ResultType, typename FilterType>
std::optional<std::reference_wrapper<ResultType>> get_component_where(std::function<bool(FilterType&)> predicate) {
    static_assert(std::is_base_of<VariantBase, ResultType>::value, "ResultType must derive from VariantBase");
    static_assert(std::is_base_of<VariantBase, FilterType>::value, "FilterType must derive from VariantBase");
    
    const rttr::type& filter_type = rttr::type::get<FilterType>();
    const rttr::type& result_type = rttr::type::get<ResultType>();
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        FilterType* filter_component = nullptr;
        for (auto& variant : variants) {
            if (variant.get_type() == filter_type) {
                filter_component = &variant.get_value<FilterType&>();
                break;
            }
        }
        
        if (filter_component && predicate(*filter_component)) {
            for (auto& variant : variants) {
                if (variant.get_type() == result_type) {
                    return std::ref(variant.get_value<ResultType&>());
                }
            }
        }
    }
    
    return std::nullopt;
}

template<typename ResultType, typename FilterType>
std::vector<std::reference_wrapper<ResultType>> find_components_where(std::function<bool(FilterType&)> predicate) {
    static_assert(std::is_base_of<VariantBase, ResultType>::value, "ResultType must derive from VariantBase");
    static_assert(std::is_base_of<VariantBase, FilterType>::value, "FilterType must derive from VariantBase");
    
    std::vector<std::reference_wrapper<ResultType>> results;
    const rttr::type& filter_type = rttr::type::get<FilterType>();
    const rttr::type& result_type = rttr::type::get<ResultType>();
    
    for (auto& [entity_id, variants] : get_zeytin().get_storage()) {
        FilterType* filter_component = nullptr;
        for (auto& variant : variants) {
            if (variant.get_type() == filter_type) {
                filter_component = &variant.get_value<FilterType&>();
                break;
            }
        }
        
        if (filter_component && predicate(*filter_component)) {
            for (auto& variant : variants) {
                if (variant.get_type() == result_type) {
                    results.push_back(std::ref(variant.get_value<ResultType&>()));
                    break;
                }
            }
        }
    }
    
    return results;
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
bool has_component(entity_id id) {
    return get_component<T>(id).has_value();
}

template<typename T>
bool has_component(const VariantBase* base) {
    return get_component<T>(base).has_value();
}

template<typename T>
bool is_active(const VariantBase* base) {
    auto component = get_component<T>(base);
    return component.has_value() && !component.value().get().is_dead;
}

inline bool entity_exists(entity_id id) {
    auto& storage = get_zeytin().get_storage();
    return storage.find(id) != storage.end();
}

template<typename T>
void remove_variant_from(entity_id id) {
    get_zeytin().remove_variant(id, rttr::type::get<T>());
}

template<typename T>
void remove_variant_from(const VariantBase* base) {
    get_zeytin().remove_variant(base->entity_id, rttr::type::get<T>());
}



} 
