#include "path_resolver/path_resolver.h"
#include "logger.h"

#define ENGINE "engine"
#define EDITOR "editor"
#define SHARED_RESOUCES "shared_resources"

void PathResolver::construct_paths() {
    std::filesystem::path current_dir = get_executable_directory();

    const int MAX_LEVELS = 10;
    int level = 0;

    while(level < MAX_LEVELS) {
        bool found_engine = std::filesystem::exists(current_dir / ENGINE);
        bool found_editor = std::filesystem::exists(current_dir / EDITOR);

        if(found_engine && found_editor) { // these 2 should always be there
            m_root_path = current_dir;

            m_engine_path = current_dir/ENGINE;
            m_editor_path = current_dir/EDITOR;
            m_resources_path = current_dir/SHARED_RESOUCES;

            bool has_resouces = std::filesystem::exists(m_resources_path);

            if(!has_resouces) { // resources folder may not be there, create one
                std::filesystem::create_directory(m_resources_path);
            }

            log_info() << "[PathResolver] Project root found at: " << m_root_path << std::endl;
            log_info() << "[PathResolver] Engine path: " << m_engine_path << std::endl;
            log_info() << "[PathResolver] Editor path: " << m_editor_path << std::endl;
            log_info() << "[PathResolver] Resources path: " << m_resources_path << std::endl;

            return; // paths are constructred
        }

        // move up one directory, if not found
        current_dir = current_dir.parent_path();
        level++;
    }

    // if we get here, we couldnt find a valid project structure
    // may crash or abort, but we also want log_error to be visible, so just log and return
    log_error() << "ERROR: Could not locate project root structure!" << std::endl;
}

std::filesystem::path PathResolver::get_executable_directory() const {
    std::filesystem::path executable_path;

#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    executable_path = buffer;
#else
    char buffer[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
    if (count != -1) {
        buffer[count] = '\0';
        executable_path = buffer;
    }
#endif

    return executable_path.parent_path();
}

std::filesystem::path PathResolver::get_resource_subdir(const std::filesystem::path& subdir) const {
    std::filesystem::path full_path = m_resources_path / subdir;

    if(!std::filesystem::exists(full_path)) {
        log_warning() << "Folder: " << full_path << " does not exist. Creating full path" << std::endl;
        std::filesystem::create_directories(full_path);
    }
    return full_path;
}

std::filesystem::path PathResolver::get_engine_subdir(const std::filesystem::path& subdir) const {
    std::filesystem::path full_path = m_engine_path / subdir;

    if(!std::filesystem::exists(full_path)) {
        log_error() << "Engine subdir: " << full_path << " does not exist." << std::endl;
    }
    return full_path;
}


