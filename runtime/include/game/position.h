#pragma once

#include <iostream>

#include "rttr/registration.h"
#include "core/entity.h"
#include "core/variant/variant_base.h"
#include "game/velocity.h"

class Position : public VariantBase {
    VARIANT(Position)

public:
    int x = 0; PROPERTY()
    int y = 0; PROPERTY()
};

