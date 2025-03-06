#include "position.h"
#include "player.h"
#include "rttr/registration.h"
#include "core/variant/variant_base.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<VariantCreateInfo>("VariantCreateInfo")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("id", &VariantCreateInfo::id);

    rttr::registration::class_<IVariantBase>("IVariantBase")
        .constructor<entity_id>()(rttr::policy::ctor::as_object)
        .property("id", &IVariantBase::id);

    rttr::registration::class_<Position>("Position")
        .constructor<entity_id>()(rttr::policy::ctor::as_object)
        .property("x", &Position::x)
        .property("y", &Position::y)
        .method("Tick", &Position::print);
}


