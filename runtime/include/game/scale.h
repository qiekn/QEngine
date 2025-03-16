#pragma once

#include "core/variant/variant_base.h"

class Scale : public VariantBase {
    VARIANT(Scale);

public:
    float x = 0; PROPERTY();
    float y = 0; PROPERTY();
};
