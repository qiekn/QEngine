#pragma once

#include "rttr/registration.h"
#include "core/variant/variant_base.h"

#include "position.h"
#include "velocity.h"


struct Player : VariantBase {
    Player() = default;
    Player(VariantCreateInfo info) : VariantBase(info) {}

    Position position;
    Velocity velocity;

    RTTR_ENABLE(VariantBase)
};
