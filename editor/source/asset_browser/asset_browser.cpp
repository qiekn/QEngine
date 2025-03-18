#include "asset_browser/asset_browser.h"

#include <cstring>
#include "imgui.h"

#include "logger/logger.h"

AssetBrowser::AssetBrowser(const std::string& root_directory)
    : m_root_directory(root_directory), m_current_directory(root_directory), m_file_watcher(root_directory)
{
    refresh();
    m_file_watcher.add_callback([this](const std::filesystem::path& file, const std::string& status) {
        on_file_changed(file, status);
    });
    m_file_watcher.start();
}

AssetBrowser::~AssetBrowser() {
    m_assets.clear();
    m_file_watcher.stop();
}

void AssetBrowser::refresh() {
    scan_directory(m_current_directory);
}

void AssetBrowser::scan_directory(const std::string& path) {
    m_assets.clear();
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        AssetItem item;
        item.path = entry.path().string();
        item.name = entry.path().filename().string();
        item.is_directory = entry.is_directory();
        item.extension = entry.path().extension().string();
        item.type = determine_asset_type(item.extension);
        m_assets.push_back(item);
    }
}

AssetType AssetBrowser::determine_asset_type(const std::string& extension) {
    static std::unordered_map<std::string, AssetType> extension_map = {
        {".png", AssetType::Image}, {".jpg", AssetType::Image},
        {".jpeg", AssetType::Image}, {".obj", AssetType::Model},
        {".fbx", AssetType::Model}, {".gltf", AssetType::Model},
        {".wav", AssetType::Audio}, {".mp3", AssetType::Audio},
        {".ogg", AssetType::Audio}, {".cpp", AssetType::Script},
        {".entity", AssetType::Entity}, {".scene", AssetType::Scene},
        {".variant", AssetType::Variant}
    };
    auto it = extension_map.find(extension);
    return (it != extension_map.end()) ? it->second : AssetType::Other;
}

void AssetBrowser::on_file_changed(const std::filesystem::path& file, const std::string& status) {
    log_trace() << "File changed: " << file << " Status: " << status << std::endl;
    refresh();
}

void AssetBrowser::render() {
    render_search_bar();
    render_directory_tree();
    render_file_list();
}

void AssetBrowser::render_search_bar() {
    static char search_buffer[256];  
    std::strncpy(search_buffer, m_search_query.c_str(), sizeof(search_buffer));  // Copy initial value

    if (ImGui::InputText("Search", search_buffer, sizeof(search_buffer))) {
        m_search_query = search_buffer;  
    }

}

void AssetBrowser::render_directory_tree() {
    if (ImGui::TreeNode(m_root_directory.c_str())) {
        for (const auto& asset : m_assets) {
            if (asset.is_directory && ImGui::TreeNode(asset.name.c_str())) {
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
}

void AssetBrowser::render_file_list() {
    for (const auto& asset : m_assets) {
        if (!asset.is_directory) {
            if (ImGui::Selectable(asset.name.c_str(), m_selected_asset == &asset)) {
                select_asset(&asset);
            }
        }
    }
}

void AssetBrowser::select_asset(const AssetItem* asset) {
    m_selected_asset = asset;
    if (m_on_asset_selected) {
        m_on_asset_selected(*asset);
    }
}

void AssetBrowser::activate_asset(const AssetItem* asset) {
    if (m_on_asset_activated) {
        m_on_asset_activated(*asset);
    }
}
