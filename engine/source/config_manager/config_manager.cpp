#include "config_manager/config_manager.h""
#include "raylib.h"

#include "remote_logger/remote_logger.h"

#include <fstream>
#include <sstream>

#include "rapidjson/prettywriter.h""
#include "resource_manager/resource_manager.h""

#include "core/raylib_wrapper.h"

static const char* config_file = "config.json";

ConfigManager::~ConfigManager() {
    // we want to save window position in editor mode everytime
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    int window_x = GetWindowPosition().x;
    int window_y = GetWindowPosition().y;

    CONFIG_SET("window_width", screen_width);
    CONFIG_SET("window_height", screen_height);
    CONFIG_SET("window_x", window_x);
    CONFIG_SET("window_y", window_y);

    save_config();
}

bool ConfigManager::load_config() {
    std::filesystem::path path = ResourceManager::get().get_resource_subdir("config") / config_file;

    if (!std::filesystem::exists(path)) {
        log_warning() << "Config file not found: " << path << std::endl;
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        log_error() << "Failed to open config file: " << path << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json_str = buffer.str();
    file.close();

    rapidjson::Document doc;
    rapidjson::ParseResult result = doc.Parse(json_str.c_str());

    if (result.IsError()) {
        log_error() << "JSON parse error in config file" << std::endl;
        return false;
    }

    if (!doc.IsObject()) {
        log_error() << "Config file must be a JSON object" << std::endl;
        return false;
    }

    m_config_values.clear();

    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        std::string key = it->name.GetString();
        ConfigValue value = json_value_to_variant(it->value);
        m_config_values[key] = value;
    }

    return true;
}

bool ConfigManager::save_config() {
    std::filesystem::path path = ResourceManager::get().get_resource_subdir("config") / config_file;

    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();

    for (const auto& [key, value] : m_config_values) {
        rapidjson::Value json_key(key.c_str(), allocator);
        rapidjson::Value json_value = variant_to_json_value(value, allocator);
        doc.AddMember(json_key, json_value, allocator);
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::ofstream file(path);
    if (!file.is_open()) {
        log_error() << "Failed to open config file for writing: " << path << std::endl;
        return false;
    }

    file << buffer.GetString();
    file.close();

    return true;
}

bool ConfigManager::has(const std::string& key) const {
    return m_config_values.find(key) != m_config_values.end();
}

void ConfigManager::remove(const std::string& key) {
    m_config_values.erase(key);
}

void ConfigManager::clear() {
    m_config_values.clear();
}

rapidjson::Value ConfigManager::variant_to_json_value(const ConfigValue& value, rapidjson::Document::AllocatorType& allocator) const {
    rapidjson::Value json_value;

    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json_value.SetInt(arg);
        } else if constexpr (std::is_same_v<T, float>) {
            json_value.SetFloat(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            json_value.SetBool(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            json_value.SetString(arg.c_str(), allocator);
        }
    }, value);

    return json_value;
}

ConfigManager::ConfigValue ConfigManager::json_value_to_variant(const rapidjson::Value& value) const {
    if (value.IsInt()) {
        return value.GetInt();
    } else if (value.IsFloat() || value.IsDouble()) {
        return value.GetFloat();
    } else if (value.IsBool()) {
        return value.GetBool();
    } else if (value.IsString()) {
        return std::string(value.GetString());
    }

    return std::string();
}
