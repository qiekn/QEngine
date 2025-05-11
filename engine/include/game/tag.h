#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "variant/variant_base.h"

class Tag : public VariantBase {
  VARIANT(Tag);

public:
  std::string value;
  PROPERTY()
};
