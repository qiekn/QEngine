#pragma once

#include "variant/variant_base.h"

class Scale : public VariantBase {
  VARIANT(Scale);

public:
  Scale(int x, int y) : x(x), y(y) {}

  float x = 1;
  PROPERTY();
  float y = 1;
  PROPERTY();
};
