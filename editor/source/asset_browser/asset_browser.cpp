#include "asset_browser/asset_browser.h"

#include <algorithm>
#include <filesystem>
#include "imgui.h"
#include "logger/logger.h"

AssetBrowser::AssetBrowser(const std::string& root_directory)
    : m_root_directory(root_directory)
{
    refresh();
}

AssetBrowser::~AssetBrowser() = default;

void AssetBrowser::refresh() {
    m_cached_tree.clear();
    build_directory_tree(m_root_directory);
}

bool AssetBrowser::build_directory_tree(const std::string& path) {
    try {
        std::string normalized_path = path;
        std::replace(normalized_path.begin(), normalized_path.end(), '\\', '/');
        
        if (!std::filesystem::exists(normalized_path)) {
            return false;
        }
        
        DirectoryNode node;
        node.path = normalized_path;
        node.name = std::filesystem::path(normalized_path).filename().string();
        if (node.name.empty() && normalized_path == m_root_directory) {
            node.name = "Zeytin";
        }
        
        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(normalized_path, ec)) {
            if (ec) {
                ec.clear();
                continue;
            }
            
            std::string entry_path = entry.path().string();
            std::replace(entry_path.begin(), entry_path.end(), '\\', '/');
            
            if (entry.is_directory()) {
                node.subdirectories.push_back(entry_path);
            } else {
                FileNode file;
                file.path = entry_path;
                file.name = entry.path().filename().string();
                file.extension = entry.path().extension().string();
                file.type = determine_asset_type(file.extension);
                node.files.push_back(file);
            }
        }
        
        std::sort(node.files.begin(), node.files.end(), 
                 [](const FileNode& a, const FileNode& b) {
                     return a.name < b.name;
                 });
        
        std::sort(node.subdirectories.begin(), node.subdirectories.end());
        
        m_cached_tree[normalized_path] = node;
        return true;
    }
    catch (const std::exception& ex) {
        log_error() << "Error building directory tree: " << ex.what() << std::endl;
        return false;
    }
}

AssetType AssetBrowser::determine_asset_type(const std::string& extension) {
    static std::unordered_map<std::string, AssetType> extension_map = {
        {".png", AssetType::Image}, {".jpg", AssetType::Image},
        {".jpeg", AssetType::Image}, {".obj", AssetType::Model},
        {".fbx", AssetType::Model}, {".gltf", AssetType::Model},
        {".wav", AssetType::Audio}, {".mp3", AssetType::Audio},
        {".ogg", AssetType::Audio}, {".cpp", AssetType::Script},
        {".h", AssetType::Script}, {".entity", AssetType::Entity}, 
        {".scene", AssetType::Scene}, {".variant", AssetType::Variant}
    };
    auto it = extension_map.find(extension);
    return (it != extension_map.end()) ? it->second : AssetType::Other;
}

void AssetBrowser::render() {
    ImGui::Text("Asset Browser");
    ImGui::Separator();
    
    if (ImGui::Button("Refresh")) {
        refresh();
    }
    
    ImGui::Separator();
    
    ImGui::BeginChild("DirectoryTree", ImVec2(0, 0), true);
    
    static std::set<std::string> expanded_nodes;
    if (expanded_nodes.empty()) {
        expanded_nodes.insert(m_root_directory);
    }
    
    render_directory(m_root_directory, expanded_nodes);
    
    ImGui::EndChild();
}

void AssetBrowser::render_directory(const std::string& path, std::set<std::string>& expanded_nodes) {
    auto it = m_cached_tree.find(path);
    if (it == m_cached_tree.end()) {
        if (!build_directory_tree(path)) {
            return;
        }
        it = m_cached_tree.find(path);
        if (it == m_cached_tree.end()) {
            return;
        }
    }
    
    const DirectoryNode& node = it->second;
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    
    if (path == m_selected_path) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    
    bool is_expanded = expanded_nodes.find(path) != expanded_nodes.end();
    if (is_expanded) {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }
    
    if (node.subdirectories.empty() && node.files.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 0.0f, 1.0f));
    bool node_open = ImGui::TreeNodeEx(("[D] " + node.name).c_str(), flags);
    ImGui::PopStyleColor();
    
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        m_selected_path = path;
        if (m_on_asset_selected) {
            AssetItem item;
            item.path = path;
            item.name = node.name;
            item.is_directory = true;
            m_on_asset_selected(item);
        }
    }
    
    if (node_open) {
        expanded_nodes.insert(path);
        
        for (const auto& file : node.files) {
            ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            
            if (file.path == m_selected_path) {
                leaf_flags |= ImGuiTreeNodeFlags_Selected;
            }
            
            std::string icon = "[F]";
            ImVec4 color(0.8f, 0.8f, 0.8f, 1.0f);
            
            switch (file.type) {
                case AssetType::Image: 
                    icon = "[I]"; 
                    color = ImVec4(0.5f, 0.8f, 1.0f, 1.0f);
                    break;
                case AssetType::Model: 
                    icon = "[M]"; 
                    color = ImVec4(0.8f, 0.5f, 1.0f, 1.0f);
                    break;
                case AssetType::Audio: 
                    icon = "[A]"; 
                    color = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
                    break;
                case AssetType::Script: 
                    icon = "[S]"; 
                    color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);
                    break;
                case AssetType::Entity: 
                    icon = "[E]"; 
                    color = ImVec4(1.0f, 0.7f, 0.2f, 1.0f);
                    break;
                case AssetType::Variant: 
                    icon = "[V]"; 
                    color = ImVec4(0.2f, 0.7f, 1.0f, 1.0f);
                    break;
                case AssetType::Scene: 
                    icon = "[C]"; 
                    color = ImVec4(1.0f, 0.5f, 0.8f, 1.0f);
                    break;
                default:
                    break;
            }
            
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TreeNodeEx((icon + " " + file.name).c_str(), leaf_flags);
            ImGui::PopStyleColor();
            
            if (ImGui::IsItemClicked()) {
                m_selected_path = file.path;
                
                if (m_on_asset_selected) {
                    AssetItem item;
                    item.path = file.path;
                    item.name = file.name;
                    item.extension = file.extension;
                    item.is_directory = false;
                    item.type = file.type;
                    m_on_asset_selected(item);
                }
            }
            
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                if (m_on_asset_activated) {
                    AssetItem item;
                    item.path = file.path;
                    item.name = file.name;
                    item.extension = file.extension;
                    item.is_directory = false;
                    item.type = file.type;
                    m_on_asset_activated(item);
                }
            }
        }
        
        for (const auto& subdir : node.subdirectories) {
            render_directory(subdir, expanded_nodes);
        }
        
        ImGui::TreePop();
    }
    else if (is_expanded) {
        expanded_nodes.erase(path);
    }
}

void AssetBrowser::set_root_directory(const std::string& path) {
    if (path == m_root_directory) {
        return;
    }
    
    std::string normalized_path = path;
    std::replace(normalized_path.begin(), normalized_path.end(), '\\', '/');
    
    if (!std::filesystem::exists(normalized_path) || !std::filesystem::is_directory(normalized_path)) {
        log_error() << "Invalid root directory: " << normalized_path << std::endl;
        return;
    }
    
    m_root_directory = normalized_path;
    m_selected_path = "";
    refresh();
}
