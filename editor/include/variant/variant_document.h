#pragma once

#include <string>
#include <filesystem>
#include <rapidjson/document.h>

#define VARIANT_FOLDER  "../shared/variants"

class VariantDocument final {

public:
    inline VariantDocument(std::string name) : m_name(name) {}
    inline VariantDocument(rapidjson::Document document, std::string name) : m_document(std::move(document)), m_name(name) {}

    inline const rapidjson::Document& get_document() const { return m_document; }
    inline const std::string& get_name() { return m_name ; }

    void load_from_file();

private:
    std::string m_name;
    rapidjson::Document m_document;
};
