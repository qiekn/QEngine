#pragma once

#include <string>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <linux/limits.h> // for PATH_MAX
#endif

#include "constants/paths.h"

class PathResolver final {
public:
    static PathResolver& get() {
        static PathResolver instance;
        return instance;
    }

    inline std::filesystem::path get_resources_path() const { return m_resources_path;}
    inline std::filesystem::path get_engine_path() const { return m_engine_path; }
    inline std::filesystem::path get_editor_path() const { return m_editor_path; }
    inline std::filesystem::path get_root_path() const { return m_root_path; }

    inline std::filesystem::path get_entity_folder() const { return get_resource_subdir(ENTITY_FOLDER); }
    inline std::filesystem::path get_variant_folder() const { return get_resource_subdir(VARIANT_FOLDER); }
    inline std::filesystem::path get_engine_scripts_folder() const { return get_engine_subdir("scripts");}

    std::filesystem::path get_resource_subdir(const std::filesystem::path& subdir) const;
    std::filesystem::path get_engine_subdir(const std::filesystem::path& subdir) const;

    void construct_paths();

private:
    PathResolver() {
        construct_paths();
    }

    std::filesystem::path get_executable_directory() const;

    std::filesystem::path m_resources_path;
    std::filesystem::path m_root_path;
    std::filesystem::path m_editor_path;
    std::filesystem::path m_engine_path;
};
