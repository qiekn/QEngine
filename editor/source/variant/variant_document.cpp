#include "variant/variant_document.h"

#include <fstream>
#include <iostream>

void VariantDocument::load_from_file() {
    if (m_name.empty()) {
        std::cerr << "Error: Cannot load variant with empty name" << std::endl;
        return;
    }

    std::filesystem::path path = std::filesystem::path(VARIANT_FOLDER) / (m_name + ".variant");

    if (!std::filesystem::exists(path)) {
        std::cerr << "Error: Variant file does not exist: " << path << std::endl;
        return;
    }

    std::ifstream in_file(path);
    if (!in_file.is_open()) {
        std::cerr << "Error: Failed to open variant file: " << path << std::endl;
        return;
    }

    std::string json_string;
    try {
        json_string = std::string(
            std::istreambuf_iterator<char>(in_file),
            std::istreambuf_iterator<char>()
        );
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to read variant file: " << path << " - " << e.what() << std::endl;
        in_file.close();
        return;
    }

    in_file.close();

    if (json_string.empty()) {
        std::cerr << "Error: Variant file is empty: " << path << std::endl;
        return;
    }

    m_document.Parse(json_string.c_str());

    if (m_document.HasParseError()) {
        std::cerr << "Error: JSON parse error in variant file: " << path << std::endl;
        std::cerr << "  Error code: " << m_document.GetParseError() << std::endl;
        std::cerr << "  Error offset: " << m_document.GetErrorOffset() << std::endl;
        return;
    }

    if (!m_document.IsObject()) {
        std::cerr << "Error: Variant file does not contain a valid JSON object: " << path << std::endl;
        return;
    }
}
