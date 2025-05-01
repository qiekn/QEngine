#pragma once

#include "raylib.h"
#include <string>
#include <filesystem>
#include <vector>

namespace image_serializer {
    std::vector<unsigned char> serialize(const Image& image);
    Image deserialize(const std::vector<unsigned char>& data);
}
