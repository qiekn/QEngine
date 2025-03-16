#pragma once

#include "core/variant/variant_base.h"

class Speed : public VariantBase { 
    VARIANT(Speed);

public:
    float value; PROPERTY();
};

