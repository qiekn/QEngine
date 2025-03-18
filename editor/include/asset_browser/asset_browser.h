#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <functional>

enum class AssetType {
    Unknown,
    Image,
    Model,
    Audio,
    Script,
    Entity,
    Variant,
    Scene,
    Other
};

struct AssetItem {
    std::string name;
    std::string path;
    std::string extension;
    bool is_directory;
    AssetType type = AssetType::Unknown;
};

class AssetBrowser {
public:
    void render();
    void set_root_directory(const std::string& path);
    void refresh();

    inline void set_on_asset_selected(std::function<void(const AssetItem&)> callback) { m_on_asset_selected = callback; }
    inline void set_on_asset_activated(std::function<void(const AssetItem&)> callback) { m_on_asset_activated = callback; }
    inline const std::string& get_selected_path() const { return m_selected_path; }

    static AssetBrowser& get() {
        static AssetBrowser instance("../");
        return instance;
    }

private:
    struct FileNode {
        std::string name;
        std::string path;
        std::string extension;
        AssetType type;
    };

    struct DirectoryNode {
        std::string name;
        std::string path;
        std::vector<FileNode> files;
        std::vector<std::string> subdirectories;
    };

    AssetBrowser(const std::string& root_directory);
    ~AssetBrowser();

    bool build_directory_tree(const std::string& path);
    void render_directory(const std::string& path, std::set<std::string>& expanded_nodes);
    AssetType determine_asset_type(const std::string& extension);

    std::string m_root_directory;
    std::string m_selected_path;
    std::unordered_map<std::string, DirectoryNode> m_cached_tree;
    
    std::function<void(const AssetItem&)> m_on_asset_selected;
    std::function<void(const AssetItem&)> m_on_asset_activated;
};
