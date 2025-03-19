#include "asset_browser/asset_browser.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>
#include "imgui.h"
#include "logger/logger.h"
#include "rlImGui.h"
#include "file_watcher/file_w.h"

AssetBrowser::AssetBrowser(const std::string& root_directory)
    : m_root_directory(root_directory), m_file_watcher_started(false)
{
    std::filesystem::create_directories(root_directory);
    refresh();
    start_file_watcher();
}

AssetBrowser::~AssetBrowser() {
    if (m_file_watcher_started && m_file_watcher) {
        m_file_watcher->stop();
    }
    clear_previews();
}

void AssetBrowser::clear_previews() {
    for (auto& preview : m_preview_cache) {
        if (preview.second.loaded) {
            UnloadTexture(preview.second.texture);
        }
    }
    m_preview_cache.clear();
}

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
            node.name = "Resources";
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
        {".png", AssetType::Image}, {".jpg", AssetType::Image}, {".jpeg", AssetType::Image}, {".bmp", AssetType::Image},
        {".wav", AssetType::Audio}, {".mp3", AssetType::Audio}, {".ogg", AssetType::Audio},
        {".cpp", AssetType::Script}, {".h", AssetType::Script}, {".sh", AssetType::Script}, 
        {".lua", AssetType::Script}, {".make", AssetType::Script},
        {".entity", AssetType::Entity}, {".scene", AssetType::Scene}, {".variant", AssetType::Variant}
    };
    auto it = extension_map.find(extension);
    return (it != extension_map.end()) ? it->second : AssetType::Other;
}

Texture2D* AssetBrowser::get_preview_texture(const std::string& path) {
    auto it = m_preview_cache.find(path);
    if (it != m_preview_cache.end()) {
        if (it->second.loaded && !it->second.failed) {
            return &it->second.texture;
        }
        return nullptr;
    }
    
    std::string extension = std::filesystem::path(path).extension().string();
    AssetType type = determine_asset_type(extension);
    
    if (type != AssetType::Image) {
        CachedPreview preview;
        preview.loaded = false;
        preview.failed = true;
        m_preview_cache[path] = preview;
        return nullptr;
    }
    
    CachedPreview preview;
    preview.loaded = false;
    preview.failed = false;
    
    try {
        preview.texture = LoadTexture(path.c_str());
        if (preview.texture.id != 0) {
            preview.loaded = true;
        } else {
            preview.failed = true;
        }
    } catch(...) {
        preview.failed = true;
    }
    
    m_preview_cache[path] = preview;
    return preview.loaded ? &m_preview_cache[path].texture : nullptr;
}

void AssetBrowser::start_file_watcher() {
    m_file_watcher = std::make_unique<FileW>(m_root_directory, std::chrono::milliseconds(500));
    
    m_file_watcher->add_callback([this](const fs::path& path, const std::string& status) {
        std::string path_str = path.string();
        std::replace(path_str.begin(), path_str.end(), '\\', '/');
        
        if (status == "created" || status == "modified") {
            if (fs::is_directory(path)) {
                build_directory_tree(path_str);
            } 
            else {
                std::string dir_path = fs::path(path).parent_path().string();
                std::replace(dir_path.begin(), dir_path.end(), '\\', '/');
                build_directory_tree(dir_path);
            }
        }
        else if (status == "deleted") {
            if (m_cached_tree.find(path_str) != m_cached_tree.end()) {
                m_cached_tree.erase(path_str);
            }
            
            std::string dir_path = fs::path(path).parent_path().string();
            std::replace(dir_path.begin(), dir_path.end(), '\\', '/');
            build_directory_tree(dir_path);
            
            auto it = m_preview_cache.find(path_str);
            if (it != m_preview_cache.end()) {
                if (it->second.loaded) {
                    //UnloadTexture(it->second.texture);
                }
                m_preview_cache.erase(it);
            }
            
            if (m_selected_path == path_str) {
                m_selected_path = "";
            }
        }
    });
    
    m_file_watcher->start();
    m_file_watcher_started = true;
}

void AssetBrowser::render() {
    ImGui::Text("Asset Browser");
    ImGui::Separator();
    
    if (ImGui::Button("Refresh")) {
        refresh();
    }
    
    ImGui::SameLine();
    ImGui::Checkbox("Show Previews", &m_show_previews);
    
    ImGui::Separator();
    
    float window_width = ImGui::GetContentRegionAvail().x;
    float tree_width = window_width * 0.5f;
    float content_width = window_width * 0.5f;
    
    ImGui::BeginChild("DirectoryTree", ImVec2(tree_width, 0), true);
    
    static std::set<std::string> expanded_nodes;
    if (expanded_nodes.empty()) {
        expanded_nodes.insert(m_root_directory);
    }
    
    try {
        render_directory(m_root_directory, expanded_nodes);
    } catch (const std::exception& e) {
        log_error() << "Asset Browser: Exception in render_directory: " << e.what() << std::endl;
    }
    
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    ImGui::BeginChild("ContentView", ImVec2(0, 0), true);
    
    if (!m_selected_path.empty()) {
        try {
            bool is_directory = std::filesystem::is_directory(m_selected_path);
            
            if (is_directory) {
                auto it = m_cached_tree.find(m_selected_path);
                if (it != m_cached_tree.end()) {
                    ImGui::Text("Current Directory: %s", m_selected_path.c_str());
                    ImGui::Separator();
                    
                    const DirectoryNode& node = it->second;
                    
                    if (ImGui::BeginTable("FilesList", m_show_previews ? 2 : 1, ImGuiTableFlags_Borders)) {
                        if (m_show_previews) {
                            ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                            ImGui::TableHeadersRow();
                        }
                        
                        for (const auto& file : node.files) {
                            ImGui::TableNextRow();
                            
                            if (m_show_previews) {
                                ImGui::TableNextColumn();
                                
                                Texture2D* texture = get_preview_texture(file.path);
                                if (texture) {
                                    float aspect = (float)texture->width / (float)texture->height;
                                    float width = 80.0f;
                                    float height = width / aspect;
                                    
                                    if (height > 80.0f) {
                                        height = 80.0f;
                                        width = height * aspect;
                                    }
                                    
                                    rlImGuiImageSize(texture, (int)width, (int)height);
                                } else {
                                    switch (file.type) {
                                        case AssetType::Image:
                                            ImGui::Text("[IMG]");
                                            break;
                                        case AssetType::Audio:
                                            ImGui::Text("[AUD]");
                                            break;
                                        case AssetType::Script:
                                            ImGui::Text("[SCR]");
                                            break;
                                        case AssetType::Entity:
                                            ImGui::Text("[ENT]");
                                            break;
                                        case AssetType::Variant:
                                            ImGui::Text("[VAR]");
                                            break;
                                        case AssetType::Scene:
                                            ImGui::Text("[SCN]");
                                            break;
                                        default:
                                            ImGui::Text("[???]");
                                            break;
                                    }
                                }
                            }
                            
                            ImGui::TableNextColumn();
                            bool is_selected = file.path == m_selected_path;
                            if (is_selected) {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
                            }
                            
                            if (ImGui::Selectable(file.name.c_str(), is_selected)) {
                                m_selected_path = file.path;
                                if (m_on_asset_selected) {
                                    AssetItem item;
                                    item.path = file.path;
                                    item.name = file.name;
                                    item.extension = file.extension;
                                    item.type = file.type;
                                    item.is_directory = false;
                                    m_on_asset_selected(item);
                                }
                            }
                            
                            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                                if (m_on_asset_activated) {
                                    AssetItem item;
                                    item.path = file.path;
                                    item.name = file.name;
                                    item.extension = file.extension;
                                    item.type = file.type;
                                    item.is_directory = false;
                                    m_on_asset_activated(item);
                                }
                            }
                            
                            if (is_selected) {
                                ImGui::PopStyleColor();
                            }
                        }
                        
                        ImGui::EndTable();
                    }
                } else {
                    log_warning() << "Directory not found in cache: " << m_selected_path << ", rebuilding..." << std::endl;
                    build_directory_tree(m_selected_path);
                }
            } else if (m_show_previews) {
                ImGui::Separator();
                
                std::filesystem::path selected_path_obj(m_selected_path);
                std::filesystem::path parent_path;
                try {
                    parent_path = selected_path_obj.parent_path();
                    
                    if (std::filesystem::exists(parent_path) && std::filesystem::is_directory(parent_path)) {
                        if (ImGui::Button("← Back to Directory")) {
                            std::string parent_path_str = parent_path.string();
                            std::replace(parent_path_str.begin(), parent_path_str.end(), '\\', '/');
                            m_selected_path = parent_path_str;
                            
                            if (m_on_asset_selected) {
                                AssetItem item;
                                item.path = parent_path_str;
                                item.name = parent_path.filename().string();
                                item.is_directory = true;
                                m_on_asset_selected(item);
                            }
                        }
                    } else {
                        log_warning() << "Asset Browser: Parent path doesn't exist or is not a directory" << std::endl;
                    }
                } catch (const std::exception& e) {
                    log_error() << "Asset Browser: Exception handling parent path: " << e.what() << std::endl;
                }
                
                ImGui::Text("Preview: %s", selected_path_obj.filename().string().c_str());
                
                std::error_code ec;
                auto file_size = std::filesystem::file_size(m_selected_path, ec);
                if (!ec) {
                    std::string size_str;
                    if (file_size < 1024) {
                        size_str = std::to_string(file_size) + " B";
                    } else if (file_size < 1024 * 1024) {
                        size_str = std::to_string(file_size / 1024) + " KB";
                    } else {
                        size_str = std::to_string(file_size / (1024 * 1024)) + " MB";
                    }
                    
                    ImGui::Text("Size: %s", size_str.c_str());
                } else {
                    log_error() << "Asset Browser: Could not get file size: " << ec.message() << std::endl;
                }
                
                std::string extension = selected_path_obj.extension().string();
                AssetType type = determine_asset_type(extension);
                
                switch (type) {
                    case AssetType::Image: {
                        Texture2D* texture = get_preview_texture(m_selected_path);
                        if (texture) {
                            ImGui::Text("Dimensions: %d x %d", texture->width, texture->height);
                            
                            ImVec2 content_size = ImGui::GetContentRegionAvail();
                            float max_width = content_size.x * 0.8f;
                            float max_height = content_size.y * 0.6f;
                            
                            float aspect = (float)texture->width / (float)texture->height;
                            float width = max_width;
                            float height = width / aspect;
                            
                            if (height > max_height) {
                                height = max_height;
                                width = height * aspect;
                            }
                            
                            rlImGuiImageSize(texture, (int)width, (int)height);
                        } else {
                            ImGui::Text("Failed to load image preview");
                        }
                        break;
                    }
                    case AssetType::Audio:
                        ImGui::Text("Audio file (no preview available)");
                        break;
                    case AssetType::Script: 
                    case AssetType::Entity:
                    case AssetType::Variant:
                    case AssetType::Scene: {
                        try {
                            std::ifstream file(m_selected_path);
                            if (file.is_open()) {
                                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                                file.close();
                                
                                ImVec2 available_size = ImGui::GetContentRegionAvail();
                                float preview_height = available_size.y - 10.0f;
                                
                                ImGui::BeginChild("TextPreview", ImVec2(0, preview_height), true, ImGuiWindowFlags_HorizontalScrollbar);
                                
                                const size_t max_preview_size = 8192;
                                std::string display_content;
                                
                                if (content.size() > max_preview_size) {
                                    display_content = content.substr(0, max_preview_size);
                                    display_content += "\n...[Content truncated, file too large to display completely]";
                                } else {
                                    display_content = content;
                                }
                                
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
                                ImGui::TextUnformatted(display_content.c_str());
                                ImGui::PopStyleColor();
                                
                                ImGui::EndChild();
                            } else {
                                ImGui::Text("Could not open file for reading");
                                log_error() << "Asset Browser: Could not open file: " << m_selected_path << std::endl;
                            }
                        } catch (const std::exception& e) {
                            ImGui::Text("Error reading file: %s", e.what());
                            log_error() << "Asset Browser: Exception reading file: " << e.what() << std::endl;
                        }
                        break;
                    }
                    default:
                        ImGui::Text("No preview available for this file type");
                        break;
                }
            }
        } catch (const std::exception& e) {
            log_error() << "Asset Browser: Exception in content view: " << e.what() << std::endl;
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: %s", e.what());
        }
    }
    
    ImGui::EndChild();
}

void AssetBrowser::render_directory(const std::string& path, std::set<std::string>& expanded_nodes) {
    auto it = m_cached_tree.find(path);
    if (it == m_cached_tree.end()) {
        if (!build_directory_tree(path)) {
            log_error() << "Asset Browser: Failed to build directory tree for: " << path << std::endl;
            return;
        }
        it = m_cached_tree.find(path);
        if (it == m_cached_tree.end()) {
            log_error() << "Asset Browser: Path still not found after building: " << path << std::endl;
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
    
    try {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.9f, 0.0f, 1.0f));
        bool node_open = ImGui::TreeNodeEx(node.name.c_str(), flags);
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
        
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Refresh")) {
                build_directory_tree(path);
            }
            
            if (ImGui::MenuItem("Create Directory")) {
            }
            
            ImGui::EndPopup();
        }
        
        if (node_open) {
            expanded_nodes.insert(path);
            
            for (const auto& subdir : node.subdirectories) {
                render_directory(subdir, expanded_nodes);
            }
            
            if (!node.files.empty()) {
                int file_idx = 0;
                for (const auto& file : node.files) {
                    ImGuiTreeNodeFlags file_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                    
                    if (file.path == m_selected_path) {
                        file_flags |= ImGuiTreeNodeFlags_Selected;
                    }
                    
                    std::string icon;
                    ImVec4 color(0.8f, 0.8f, 0.8f, 1.0f);
                    
                    switch (file.type) {
                        case AssetType::Image:
                            icon = "[IMG] "; 
                            color = ImVec4(0.5f, 0.8f, 1.0f, 1.0f);
                            break;
                        case AssetType::Audio:
                            icon = "[AUD] "; 
                            color = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
                            break;
                        case AssetType::Script:
                            icon = "[SCR] "; 
                            color = ImVec4(0.5f, 1.0f, 0.5f, 1.0f);
                            break;
                        case AssetType::Entity:
                            icon = "[ENT] "; 
                            color = ImVec4(1.0f, 0.7f, 0.2f, 1.0f);
                            break;
                        case AssetType::Variant:
                            icon = "[VAR] "; 
                            color = ImVec4(0.2f, 0.7f, 1.0f, 1.0f);
                            break;
                        case AssetType::Scene:
                            icon = "[SCN] "; 
                            color = ImVec4(1.0f, 0.5f, 0.8f, 1.0f);
                            break;
                        default:
                            icon = "[FILE] ";
                            break;
                    }
                    
                    ImGui::PushID(file_idx++);
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TreeNodeEx((icon + file.name).c_str(), file_flags);
                    ImGui::PopStyleColor();
                    
                    if (ImGui::IsItemClicked()) {
                        m_selected_path = file.path;
                        
                        if (m_on_asset_selected) {
                            AssetItem item;
                            item.path = file.path;
                            item.name = file.name;
                            item.extension = file.extension;
                            item.type = file.type;
                            item.is_directory = false;
                            m_on_asset_selected(item);
                        }
                    }
                    
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        if (m_on_asset_activated) {
                            AssetItem item;
                            item.path = file.path;
                            item.name = file.name;
                            item.extension = file.extension;
                            item.type = file.type;
                            item.is_directory = false;
                            m_on_asset_activated(item);
                        }
                    }
                    
                    ImGui::PopID();
                }
            }
            
            ImGui::TreePop();
        }
        else if (is_expanded) {
            expanded_nodes.erase(path);
        }
    }
    catch (const std::exception& e) {
        log_error() << "Asset Browser: Exception in render_directory for " << path << ": " << e.what() << std::endl;
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
