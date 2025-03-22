#pragma once

#include "core/variant/variant_base.h"

class Scale : public VariantBase {
    VARIANT(Scale);

public:
    float x = 1; PROPERTY();
    float y = 1; PROPERTY();
};
