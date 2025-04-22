#pragma once

#include "variant/variant_base.h"
#include <string>
#include <unordered_map>
#include <vector>

class Tag : public VariantBase {
    VARIANT(Tag);

public:
    std::string value; PROPERTY()
};
