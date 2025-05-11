#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "entity/entity.h"
#include "rapidjson/document.h"

class Scene {
public:
  Scene() = default;
  ~Scene() = default;

  static bool load_from_file(const std::filesystem::path& path);
  static bool save_to_file(const std::filesystem::path& path);
};
