#include "entity/entity_document.h"

#include <fstream>
#include <iostream>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

void EntityDocument::save_to_file() const {
    std::filesystem::create_directories(ENTITY_FOLDER);
    std::filesystem::path path = std::filesystem::path(ENTITY_FOLDER) / (m_name + ".entity");
    std::ofstream out_file(path);

    if (!out_file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    if (!m_document.Accept(writer)) {
        std::cerr << "Failed to serialize JSON document" << std::endl;
        return;
    }

    out_file << buffer.GetString();

    if (out_file.fail()) {
        std::cerr << "Failed to write to file: " << path << std::endl;
    }

    out_file.close();
}

void EntityDocument::load_from_file() {
    std::filesystem::create_directories(ENTITY_FOLDER);
    std::filesystem::path path = std::filesystem::path(ENTITY_FOLDER) / (m_name + ".entity");
   
    std::ifstream in_file(path);
   
    if (!in_file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }
   
    std::string json_string((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
    in_file.close();
    
    m_document.Parse(json_string.c_str());
    
    if (m_document.HasParseError()) {
        std::cerr << "JSON parse error at offset " << m_document.GetErrorOffset() << ": " 
                  << m_document.GetParseError() << std::endl;
    }
}

void EntityDocument::save_to_file(const std::filesystem::path& path) const {
    std::cout << "Save to file is called" << std::endl;

    std::filesystem::create_directories(path.parent_path());
    
    std::ofstream out_file(path);
    
    if (!out_file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return;
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    
    if (!m_document.Accept(writer)) {
        std::cerr << "Failed to serialize JSON document" << std::endl;
        return;
    }
    
    out_file << buffer.GetString();
    
    if (out_file.fail()) {
        std::cerr << "Failed to write to file: " << path << std::endl;
    }

    std::cout << buffer.GetString();
    
    out_file.close();
}

void EntityDocument::load_from_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "File does not exist: " << path << std::endl;
        return;
    }
    
    std::ifstream in_file(path);
    
    if (!in_file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }
    
    std::string json_string((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
    in_file.close();
    
    m_document.Parse(json_string.c_str());
    
    if (m_document.HasParseError()) {
        std::cerr << "JSON parse error at offset " << m_document.GetErrorOffset() << ": " 
                  << m_document.GetParseError() << std::endl;
    }
}


void EntityDocument::delete_entity_file() {
    std::filesystem::path path = std::filesystem::path(ENTITY_FOLDER) / (m_name + ".entity");
    std::filesystem::remove(path);
}








