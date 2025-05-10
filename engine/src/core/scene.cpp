#include "core/scene.h"
#include "core/zeytin.h"
#include "remote_logger/remote_logger.h"

#include <fstream>

bool Scene::load_from_file(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        log_error() << "Cannot load scene: Path " << path << " does not exist" << std::endl;
        return false;
    }

    std::ifstream scene_file(path);
    std::string scene_data((std::istreambuf_iterator<char>(scene_file)),
                            std::istreambuf_iterator<char>());
    scene_file.close();

    if(Zeytin::get().deserialize_scene(scene_data)) {
        log_info() << "Scene loaded successfully: " << path << std::endl;
        return true;
    }
        
    return false;
}

bool Scene::save_to_file(const std::filesystem::path& path) {
    std::filesystem::create_directories(path.parent_path());
        
    std::string scene_data = Zeytin::get().serialize_scene();
    if (scene_data.empty()) {
        log_error() << "Failed to serialize scene" << std::endl;
        return false;
    }
        
    std::ofstream out_file(path);
    if (!out_file.is_open()) {
        log_error() << "Failed to open file for writing: " << path << std::endl;
        return false;
    }
        
    out_file << scene_data;
        
    if (out_file.fail()) {
        log_error() << "Failed to write to file: " << path << std::endl;
        return false;
    }
        
    out_file.close();
    
    log_info() << "Scene saved successfully: " << path << std::endl;
    return true;
}
