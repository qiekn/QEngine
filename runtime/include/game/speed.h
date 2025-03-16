#pragma once

#include "core/variant/variant_base.h"

class Speed : public VariantBase { 
    VARIANT(Speed);

public:
    float value; PROPERTY();
    std::string unit_speed; PROPERTY();
};

