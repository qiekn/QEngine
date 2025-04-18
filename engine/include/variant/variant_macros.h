#include "core/zeytin.h"
#include "remote_logger/remote_logger.h"

#define PROPERTY() 
#define DEBUG()

#define SET_CALLBACK(callback_name) \
    void callback_name();

#define VARIANT(ClassName) \
public: \
    ClassName() = default; \
    ClassName(VariantCreateInfo info) : VariantBase(info) {} \
    RTTR_ENABLE(VariantBase); \
    static constexpr const char* get_variant_name() { return #ClassName; } \
private:

template<typename T>
std::optional<std::reference_wrapper<T>> get(entity_id id) {
    auto& variants = get_zeytin().get_variants(id);
    
    for (auto& variant : variants) {
        if (variant.get_type() == rttr::type::get<T>()) {
            return std::ref(variant.get_value<T&>());
        }
    }
    
    return std::nullopt;
}

template<typename T>
bool has(entity_id id) {
    return get<T>(id).has_value();
}

#define REQUIRES(...) \
    bool check_dependencies(const std::string& method_name) const override { \
        return check_dependencies_impl<__VA_ARGS__>(method_name); \
    } \
    template<typename T, typename... Rest> \
    bool check_dependencies_impl(const std::string& method_name) const { \
        bool has_all = true; \
        if (!has<T>(this->entity_id)) { \
            log_error() << rttr::type::get<T>().get_name() << " variant is required by " << this->get_variant_name() \
                        << " variant but not found on entity id: " << entity_id << ". Method will not be called: " << method_name << std::endl; \
            has_all = false; \
        } \
        if constexpr (sizeof...(Rest) > 0) { \
            has_all = check_dependencies_impl<Rest...>(method_name) && has_all; \
        } \
        return has_all; \
    } \

template<typename... Ts>
bool _check_components_present(entity_id id) {
    return (has<Ts>(id) && ...);
}
