#include "image_serializer//image_serializer.h"
#include <fstream>
#include <iostream>
#include "remote_logger/remote_logger.h"
#include <cstring>

namespace image_serializer {

std::vector<unsigned char> serialize(const Image& image) {
    // format: [width:int32][height:int32][format:int32][mipmaps:int32][data_size:int32][data:bytes]
    std::vector<unsigned char> result;
    
    int bytes_per_pixel;
    switch (image.format) {
        case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: bytes_per_pixel = 1; break;
        case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: bytes_per_pixel = 2; break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8: bytes_per_pixel = 3; break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: bytes_per_pixel = 4; break;
        case PIXELFORMAT_UNCOMPRESSED_R32: bytes_per_pixel = 4; break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32: bytes_per_pixel = 12; break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: bytes_per_pixel = 16; break;
        case PIXELFORMAT_UNCOMPRESSED_R16: bytes_per_pixel = 2; break;
        case PIXELFORMAT_UNCOMPRESSED_R16G16B16: bytes_per_pixel = 6; break;
        case PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: bytes_per_pixel = 8; break;
        default: bytes_per_pixel = 4; break; 
    }
    
    const size_t header_size = 5 * sizeof(int);
    const size_t data_size = image.width * image.height * bytes_per_pixel;
    result.reserve(header_size + data_size);
    
    auto append_int = [&result](int value) {
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&value);
        result.insert(result.end(), bytes, bytes + sizeof(int));
    };
    
    append_int(image.width);
    append_int(image.height);
    append_int(image.format);
    append_int(image.mipmaps);
    append_int(static_cast<int>(data_size));
    
    const unsigned char* pixel_data = static_cast<const unsigned char*>(image.data);
    result.insert(result.end(), pixel_data, pixel_data + data_size);
    
    return result;
}

Image deserialize(const std::vector<unsigned char>& data) {
    if (data.size() < 5 * sizeof(int)) {
        log_error() << "Invalid image data format for deserialization" << std::endl;
        return {};
    }
    
    auto read_int = [&data](size_t offset) -> int {
        return *reinterpret_cast<const int*>(data.data() + offset);
    };

    size_t offset = 0;
    int width = read_int(offset); offset += sizeof(int);
    int height = read_int(offset); offset += sizeof(int);
    int format = read_int(offset); offset += sizeof(int);
    int mipmaps = read_int(offset); offset += sizeof(int);
    int data_size = read_int(offset); offset += sizeof(int);
    
    if (offset + data_size > data.size()) {
        log_error() << "Incomplete image data for deserialization" << std::endl;
        return {};
    }
    
    Image image = { 
        .data = nullptr,
        .width = width,
        .height = height,
        .mipmaps = mipmaps,
        .format = format 
    };
    
    int bytes_per_pixel;
    switch (format) {
        case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: bytes_per_pixel = 1; break;
        case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: bytes_per_pixel = 2; break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8: bytes_per_pixel = 3; break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: bytes_per_pixel = 4; break;
        case PIXELFORMAT_UNCOMPRESSED_R32: bytes_per_pixel = 4; break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32: bytes_per_pixel = 12; break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: bytes_per_pixel = 16; break;
        case PIXELFORMAT_UNCOMPRESSED_R16: bytes_per_pixel = 2; break;
        case PIXELFORMAT_UNCOMPRESSED_R16G16B16: bytes_per_pixel = 6; break;
        case PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: bytes_per_pixel = 8; break;
        default: bytes_per_pixel = 4; break; // default to RGBA 4 bytes
    }
    
    size_t expected_data_size = width * height * bytes_per_pixel;
    if (data_size != expected_data_size) {
        log_warning() << "Image data size mismatch: stored=" << data_size 
                    << ", calculated=" << expected_data_size << std::endl;
    }
    
    image.data = MemAlloc(data_size);
    if (!image.data) {
        log_error() << "Failed to allocate memory for image data" << std::endl;
        return {};
    }
    
    memcpy(image.data, data.data() + offset, data_size);
    
    return image;
}
}

