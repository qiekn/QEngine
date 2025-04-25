#pragma once

#include <string>
#include <filesystem>


#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <linux/limits.h> // for PATH_MAX
#endif

#define ENTITY_FOLDER "entities"
#define VARIANT_FOLDER  "variants"
#define ENGINE_SCRIPTS_FOLDER "scripts"

#define get_resource_manager() ResourceManager::get()

class ResourceManager final {
public:
    static ResourceManager& get() {
        static ResourceManager instance;
        return instance;
    }

    inline std::filesystem::path get_resources_path() const { return m_resources_path;}
    inline std::filesystem::path get_engine_path() const { return m_engine_path; }
    inline std::filesystem::path get_editor_path() const { return m_editor_path; }
    inline std::filesystem::path get_root_path() const { return m_root_path; }

    inline std::filesystem::path get_entities_path() const { return get_resource_subdir(ENTITY_FOLDER); }
    inline std::filesystem::path get_variants_path() const { return get_resource_subdir(VARIANT_FOLDER); }
    inline std::filesystem::path get_engine_scripts_path() const { return get_engine_subdir(ENGINE_SCRIPTS_FOLDER);}

    std::filesystem::directory_iterator get_entity_folder() const;
    std::filesystem::directory_iterator get_variant_folder() const;

    std::filesystem::path get_resource_subdir(const std::filesystem::path& subdir) const;
    std::filesystem::path get_engine_subdir(const std::filesystem::path& subdir) const;

    std::filesystem::path get_variant_path(const std::string& name) const;
    std::filesystem::path get_entity_path(const std::string& name) const;


private:
    ResourceManager() {
        construct_paths();
    }

    void construct_paths();
    std::filesystem::path get_executable_directory() const;

    std::filesystem::path m_resources_path;
    std::filesystem::path m_root_path;
    std::filesystem::path m_editor_path;
    std::filesystem::path m_engine_path;
};

