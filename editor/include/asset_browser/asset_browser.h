#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>
#include <functional>
#include <unordered_map>

#include "imgui.h"
#include "raylib.h"

#include "file_watcher/file_w.h"

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
    inline const AssetItem* get_selected_asset() const { return m_selected_asset; }
    inline const std::string& get_current_directory() const { return m_current_directory; }

    static AssetBrowser& get() {
        static AssetBrowser instance(".");
        return instance;
    }

private:
    AssetBrowser(const std::string& root_directory);
    ~AssetBrowser();

    void scan_directory(const std::string& path);
    void render_breadcrumbs();
    void render_directory_tree();
    void render_file_list();
    void render_search_bar();
    void navigate_to_directory(const std::string& path);
    void select_asset(const AssetItem* asset);
    void activate_asset(const AssetItem* asset);
    bool filter_asset(const AssetItem& asset);

    Texture2D generate_preview_for_file(const std::string& file_path);
    void on_file_changed(const std::filesystem::path& file, const std::string& status);
    AssetType determine_asset_type(const std::string& extension);

    std::string m_root_directory;
    std::string m_current_directory;
    std::vector<AssetItem> m_assets;
    std::vector<AssetItem> m_filtered_assets;
    std::map<std::string, std::vector<AssetItem>> m_directory_map;
    const AssetItem* m_selected_asset = nullptr;
    std::string m_search_query;
    bool m_show_directory_tree = true;
    ImVec2 m_thumbnail_size = ImVec2(64, 64);
    std::vector<std::string> m_breadcrumbs;

    std::function<void(const AssetItem&)> m_on_asset_selected;
    std::function<void(const AssetItem&)> m_on_asset_activated;

    FileW m_file_watcher;
};

