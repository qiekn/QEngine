#include "window/window_manager.h"
#include "logger.h"
#include <unordered_map>
#include "imgui_internal.h""

WindowManager::WindowManager() 
    : m_main_dockspace_id(0)
    , m_first_layout(true)
{
}

void WindowManager::init() {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void WindowManager::render() {
    create_dockspace();
    render_main_menu_bar();
    
    for (auto& window : m_windows) {
        if (!window.is_open)
            continue;
        
        if (ImGui::Begin(window.menu_info.name.c_str(), &window.is_open, window.flags)) {
            if (window.render_func) {
                window.render_func();
            }
            ImGui::End();
        }
    }
}

void WindowManager::add_window(const std::string& name, 
                              std::function<void()> render_func,
                              bool default_open,
                              const std::string& menu_path,
                              bool visible_in_menu,
                              ImGuiWindowFlags flags) {
    MenuInfo menu_info;
    menu_info.name = name;
    menu_info.menu_path = menu_path;
    menu_info.visible_in_menu = visible_in_menu;
    menu_info.default_open = default_open;
    
    WindowInfo window_info;
    window_info.menu_info = menu_info;
    window_info.render_func = render_func;
    window_info.is_open = default_open;
    window_info.flags = flags;
    
    m_windows.push_back(window_info);
}

void WindowManager::create_dockspace() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    
    m_main_dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(m_main_dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    
    if (m_first_layout && !m_windows.empty()) {
        m_first_layout = false;
        
        ImGui::DockBuilderRemoveNode(m_main_dockspace_id);
        ImGui::DockBuilderAddNode(m_main_dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(m_main_dockspace_id, viewport->Size);
        
        ImGuiID dock_main_id = m_main_dockspace_id;
        ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
        ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);
        ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);
        
        ImGui::DockBuilderFinish(m_main_dockspace_id);
    }
    
    ImGui::End();
}

void WindowManager::render_main_menu_bar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save All", "Ctrl+S")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {}
            ImGui::EndMenu();
        }
        
        std::unordered_map<std::string, std::vector<std::pair<std::string, bool*>>> menu_paths;
        
        for (auto& window : m_windows) {
            if (window.menu_info.visible_in_menu) {
                menu_paths[window.menu_info.menu_path].push_back({window.menu_info.name, &window.is_open});
            }
        }
        
        for (const auto& [menu_path, items] : menu_paths) {
            if (ImGui::BeginMenu(menu_path.c_str())) {
                for (const auto& [name, is_open] : items) {
                    ImGui::MenuItem(name.c_str(), nullptr, is_open);
                }
                ImGui::EndMenu();
            }
        }
        
        ImGui::EndMainMenuBar();
    }
}

void WindowManager::handle_menu_item(const std::string& menu_path, const std::string& name, bool& is_open) {
    if (ImGui::MenuItem(name.c_str(), nullptr, &is_open)) {}
}
