#include "variant/variant_document.h"

#include <fstream>
#include <iostream>

void VariantDocument::load_from_file() {
    std::filesystem::path path = std::filesystem::path(VARIANT_FOLDER) / (m_name + ".variant");

    std::cout << "Loading variant from : " << path << std::endl;

    std::ifstream in_file(path);
    assert(in_file.is_open());

    std::string json_string((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
    in_file.close();

    m_document.Parse(json_string.c_str());
    assert(!m_document.HasParseError());
}
