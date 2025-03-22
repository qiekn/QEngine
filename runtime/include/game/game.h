#pragma once

#include "core/variant/variant_base.h"
#include "raylib.h"

class ZCamera : public VariantBase {
    VARIANT(ZCamera);
public:
    Camera2D ray_cam; PROPERTY();
};
