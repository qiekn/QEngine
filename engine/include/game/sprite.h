#pragma once

#include "game/position.h"
#include "game/scale.h"
#include "raylib.h"
#include "variant/variant_base.h"

class Sprite : public VariantBase {
  VARIANT(Sprite);

public:
  std::string path_to_sprite;
  PROPERTY() SET_CALLBACK(handle_new_path);

  void on_init() override;
  void on_update() override;

private:
  void basic_move();

  Texture texture;
  bool m_texture_loaded = false;
};
