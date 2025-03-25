#include "core/zeytin.h"
#include "remote_logger/remote_logger.h"

#define PROPERTY() 
#define SET_CALLBACK(property_name) \
    void on_##property_name##_set();

#define VARIANT(ClassName) \
public: \
    ClassName() = default; \
    ClassName(VariantCreateInfo info) : VariantBase(info) {} \
    RTTR_ENABLE(VariantBase); \
    static constexpr const char* get_variant_name() { return #ClassName; } \
private:

template<typename T>
std::optional<std::reference_wrapper<T>> get_component(entity_id id) {
    auto& variants = get_zeytin().get_variants(id);
    
    for (auto& variant : variants) {
        if (variant.get_type() == rttr::type::get<T>()) {
            return std::ref(variant.get_value<T&>());
        }
    }
    
    return std::nullopt;
}

template<typename T>
bool has_component(entity_id id) {
    return get_component<T>(id).has_value();
}

#define REQUIRES(...) \
    bool check_dependencies() const override { \
        return _check_dependencies<__VA_ARGS__>(); \
    } \
    template<typename T, typename... Rest> \
    bool _check_dependencies() const { \
        if (!has_component<T>(this->entity_id)) { \
            log_error() << rttr::type::get<T>().get_name() << " variant is required by " << this->get_variant_name() \
                        << " variant but not found on entity id: " << entity_id; \
            return false; \
        } \
        if constexpr (sizeof...(Rest) > 0) { \
            return _check_dependencies<Rest...>(); \
        } \
        return true; \
}
