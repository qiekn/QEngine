#include "test_viewer/test_viewer.h"
#include "imgui.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <cctype>

namespace Test {

TestViewer::TestViewer() 
{
    for(auto& kv : expanded_tests) {
        kv.second = false;
    }
}

TestViewer::~TestViewer() 
{
}

void TestViewer::render()
{
    if (!is_test_loaded()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No test plan loaded.");
        return;
    }

    ImGui::BeginChild("test_summary", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 5), true);
    render_test_summary();
    ImGui::EndChild();

    ImGui::Separator();

    ImGui::BeginChild("test_details_panel", ImVec2(0, 0), true);
    for (int i = 0; i < test_plan.test_cases.size(); i++) {
        render_single_test_case(i);

        if (i < test_plan.test_cases.size() - 1) {
            ImGui::Separator();
        }
    }
    ImGui::EndChild();
}

void TestViewer::render_single_test_case(int index)
{
    auto& test = test_plan.test_cases[index];

    ImGui::PushID(index);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    std::string header = "Test " + std::to_string(index + 1);

    bool is_open = ImGui::CollapsingHeader(header.c_str(),
                                          0);

    ImGui::PopStyleColor();

    ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
    ImGui::Text("Status: ");
    ImGui::SameLine();

    if (test.is_executed) {
        ImGui::TextColored(get_test_result_color(test.actual_result.type),
                          "%s", get_test_result_string(test.actual_result.type).c_str());
    } else {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Not Executed");
    }

    if (is_open) {
        ImGui::Columns(2, ("test_columns_" + std::to_string(index)).c_str(), false);

        ImGui::TextColored(ImVec4(0.7f, 0.9f, 1.0f, 1.0f), "Action:");
        ImGui::BeginChild(("action_text_" + std::to_string(index)).c_str(),
                         ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 3), true);
        ImGui::TextWrapped("%s", test.action.value.c_str());
        ImGui::EndChild();

        ImGui::NextColumn();

        ImGui::TextColored(ImVec4(0.7f, 0.9f, 1.0f, 1.0f), "Expected Result:");
        ImGui::BeginChild(("expected_result_text_" + std::to_string(index)).c_str(),
                         ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 3), true);
        ImGui::TextWrapped("%s", test.expected_result.value.c_str());
        ImGui::EndChild();

        ImGui::Columns(1);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.7f, 0.9f, 1.0f, 1.0f), "Actual Result:");

        if (actual_result_buffers.find(index) == actual_result_buffers.end()) {
            actual_result_buffers[index] = std::vector<char>(4096, 0);
            if (!test.actual_result.value.empty()) {
                strncpy(actual_result_buffers[index].data(), test.actual_result.value.c_str(),
                        actual_result_buffers[index].size() - 1);
            }
        }

        std::string buffer_id = "##actual_result_" + std::to_string(index);
        if (ImGui::InputTextMultiline(buffer_id.c_str(),
                                   actual_result_buffers[index].data(),
                                   actual_result_buffers[index].size(),
                                   ImVec2(-1, ImGui::GetTextLineHeightWithSpacing() * 4),
                                   ImGuiInputTextFlags_AllowTabInput)) {
            test.actual_result.value = actual_result_buffers[index].data();
        }

        ImGui::Spacing();
        ImGui::Text("Set Result:");

        float button_width = std::min(80.0f, ImGui::GetContentRegionAvail().x / 6.0f);

        std::unordered_map<ResultType, ImVec4> softened_colors;
        softened_colors[ResultType::Todo] = ImVec4(0.5f, 0.5f, 0.7f, 0.7f);
        softened_colors[ResultType::Passed] = ImVec4(0.0f, 0.6f, 0.0f, 0.7f);
        softened_colors[ResultType::Failed] = ImVec4(0.7f, 0.0f, 0.0f, 0.7f);
        softened_colors[ResultType::Blocked] = ImVec4(0.7f, 0.4f, 0.0f, 0.7f);
        softened_colors[ResultType::Skipped] = ImVec4(0.5f, 0.5f, 0.5f, 0.7f);
        softened_colors[ResultType::Acceptable] = ImVec4(0.7f, 0.6f, 0.0f, 0.7f);

        for (int i = 1; i < static_cast<int>(ResultType::Length); i++) {
            const ResultType result_type = static_cast<ResultType>(i);
            const std::string& name = get_test_result_string(result_type);

            if (i > 0) ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, softened_colors[result_type]);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                ImVec4(softened_colors[result_type].x * 1.2f,
                                       softened_colors[result_type].y * 1.2f,
                                       softened_colors[result_type].z * 1.2f, 0.8f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // White text for contrast

            if(ImGui::Button(name.c_str(), ImVec2(button_width, 0))) {
                test.is_executed = true;
                test.actual_result.type = result_type;
                update_test_statistics();
            }

            ImGui::PopStyleColor(3);
        }

    }

    ImGui::PopID();
}

void TestViewer::update_test_statistics()
{
    tests_executed = 0;
    tests_passed = 0;
    tests_failed = 0;
    tests_blocked = 0;
    tests_skipped = 0;
    tests_acceptable = 0;

    for (const auto& test : test_plan.test_cases) {
        if (test.is_executed) {
            tests_executed++;

            switch (test.actual_result.type) {
                case ResultType::Passed: tests_passed++; break;
                case ResultType::Failed: tests_failed++; break;
                case ResultType::Blocked: tests_blocked++; break;
                case ResultType::Skipped: tests_skipped++; break;
                case ResultType::Acceptable: tests_acceptable++; break;
                default: break;
            }
        }
    }
}

void TestViewer::render_test_list() 
{
    ImGui::Text("Test Cases:");
    
    ImGui::SetNextItemWidth(-1);
    
    ImGui::Separator();
    
    for (int i = 0; i < test_plan.test_cases.size(); i++) {
        const auto& test = test_plan.test_cases[i];
        
        std::string label = "Test " + std::to_string(i + 1);
        if (test.is_executed) {
            label += " [" + get_test_result_string(test.actual_result.type) + "]";
        }
        
        if (test.is_executed) {
            ImGui::PushStyleColor(ImGuiCol_Text, get_test_result_color(test.actual_result.type));
        }
        
        bool is_selected = (i == selected_test_index);
        if (ImGui::Selectable(label.c_str(), is_selected)) {
            selected_test_index = i;
            log_info() << "Selected: " << i << std::endl;
        }
        
        if (test.is_executed) {
            ImGui::PopStyleColor();
        }
    }
}

void TestViewer::render_test_details() 
{
    if (selected_test_index < 0 || selected_test_index >= test_plan.test_cases.size()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select a test to view details.");
        return;
    }
    
    const auto& test = test_plan.test_cases[selected_test_index];
    
    ImGui::Text("Test %d Details:", selected_test_index + 1);
    ImGui::Separator();
    
    ImGui::Text("Status: ");
    ImGui::SameLine();
    
    if (test.is_executed) {
        ImGui::TextColored(get_test_result_color(test.actual_result.type), "%s", get_test_result_string(test.actual_result.type).c_str());
    } else {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Not Executed");
    }
    
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Action", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginChild("action_text", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.25f), true);
        ImGui::TextWrapped("%s", test.action.value.c_str());
        ImGui::EndChild();
    }
    
    if (ImGui::CollapsingHeader("Expected Result", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginChild("expected_result_text", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.25f), true);
        ImGui::TextWrapped("%s", test.expected_result.value.c_str());
        ImGui::EndChild();
    }
    
    ImGui::Separator();

    for(int i = 1; i < static_cast<int>(ResultType::Length); ++i) {
        const std::string& name = get_test_result_string(static_cast<ResultType>(i));
        const std::string& label = "Mark as " + name;

        ImGui::SameLine();
        if(ImGui::Button(label.c_str())) {
            auto& current_case = test_plan.test_cases[selected_test_index];
            current_case.is_executed = true;
            current_case.actual_result.type = static_cast<ResultType>(i);
        }
    }
    
    static char notes[4096] = "";
    ImGui::Separator();
    ImGui::Text("Notes:");
    ImGui::InputTextMultiline("##notes", notes, sizeof(notes), ImVec2(-1, ImGui::GetContentRegionAvail().y), ImGuiInputTextFlags_AllowTabInput);
}

void TestViewer::render_test_summary() 
{
    ImGui::Text("Summary: %d/%d Tests Executed", tests_executed, (int)test_plan.test_cases.size());
    
    ImGui::Columns(5, "test_summary_columns", false);
    
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "Passed: %d", tests_passed);
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed: %d", tests_failed);
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Blocked: %d", tests_blocked);
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Skipped: %d", tests_skipped);
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Acceptable: %d", tests_acceptable);
    ImGui::NextColumn();
    
    ImGui::Columns(1);

    
    if (tests_executed > 0) {
        float pass_percentage = (float)(tests_passed + tests_acceptable) / test_plan.test_cases.size() * 100.0f;
        ImGui::ProgressBar((float)tests_executed / test_plan.test_cases.size(), ImVec2(-1, 10.0f), "");
        ImGui::Text("%.1f%% Pass Rate (including acceptable issues)", pass_percentage);
    }
}

void TestViewer::load_test_file(const std::string& file_path) 
{
    test_plan.test_cases.clear();
    test_plan.name = std::filesystem::path(file_path).stem().string();
    
    selected_test_index = -1;
    
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            log_error() << "Failed to open test file: " << file_path << std::endl;
            return;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        parse_csv(content);
        
        log_info() << "Loaded " << test_plan.test_cases.size() << " test cases from " << file_path << std::endl;
    } 
    catch (std::exception& e) {
        log_error() << "Exception while loading test file: " << e.what() << std::endl;
    }
}

void TestViewer::save_results(const std::string& file_path) 
{
    try {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            log_error() << "Failed to open file for saving results: " << file_path << std::endl;
            return;
        }
        
        file << "Test ID,Action,Expected Result,Actual Result,Status\n";
        
        for (const auto& test : test_plan.test_cases) {
            auto escape_csv_field = [](const std::string& field) -> std::string {
                if (field.find(',') != std::string::npos || field.find('"') != std::string::npos || field.find('\n') != std::string::npos) {
                    std::string escaped = field;
                    // Replace " with ""
                    size_t start_pos = 0;
                    while ((start_pos = escaped.find("\"", start_pos)) != std::string::npos) {
                        escaped.replace(start_pos, 1, "\"\"");
                        start_pos += 2;
                    }
                    return "\"" + escaped + "\"";
                }
                return field;
            };
            
            file 
                 << escape_csv_field(test.action.value) << ","
                 << escape_csv_field(test.expected_result.value) << ",";
            
            if (test.is_executed) {
                file << escape_csv_field(test.actual_result.value) << ",";
                file << get_test_result_string(test.actual_result.type);
            } else {
                file << "," << "Not Executed";
            }
            
            file << "\n";
        }
        
        file.close();
        log_info() << "Saved test results to " << file_path << std::endl;
    }
    catch (std::exception& e) {
        log_error() << "Exception while saving test results: " << e.what() << std::endl;
    }
}

void TestViewer::parse_csv(const std::string& content) 
{
    std::istringstream stream(content);
    std::string line;
    
    if (std::getline(stream, line)) {
        if (line.find("Action") != std::string::npos && line.find("Expected Result") != std::string::npos) {
        } else {
            process_csv_line(line);
        }
    }
    
    int test_id = 1;
    while (std::getline(stream, line)) {
        TestCase test_case = process_csv_line(line);
        test_plan.test_cases.push_back(test_case);
    }
}

TestCase TestViewer::process_csv_line(const std::string& line) 
{
    TestCase test_case;
    
    size_t comma_pos = line.find(',');
    if (comma_pos != std::string::npos) {
        test_case.action.value = line.substr(0, comma_pos);
        test_case.expected_result.value = line.substr(comma_pos + 1);
        
        if (test_case.action.value.size() > 0 && test_case.action.value[0] == '"') {
            size_t end_quote = 1;
            while (end_quote < test_case.action.value.size()) {
                if (test_case.action.value[end_quote] == '"') {
                    if (end_quote + 1 < test_case.action.value.size() && test_case.action.value[end_quote + 1] == '"') {
                        end_quote += 2; 
                    } else {
                        break; 
                    }
                } else {
                    end_quote++;
                }
            }
            
            if (end_quote < test_case.action.value.size()) {
                test_case.action.value = test_case.action.value.substr(1, end_quote - 1);
                size_t pos = 0;
                while ((pos = test_case.action.value.find("\"\"", pos)) != std::string::npos) {
                    test_case.action.value.replace(pos, 2, "\"");
                    pos += 1;
                }
            }
        }
        
        if (test_case.expected_result.value.size() > 0 && test_case.expected_result.value[0] == '"') {
        }
    }
    
    return test_case;
}


void TestViewer::reset_tests() 
{
    for (auto& test : test_plan.test_cases) {
        test.is_executed = false;
        test.actual_result.type = ResultType::None;
        test.actual_result.value.clear();
    }
}


std::string TestViewer::get_test_result_string(ResultType result) const 
{
    switch (result) {
        case ResultType::None: return "None";
        case ResultType::Todo: return "Todo";
        case ResultType::Passed: return "Passed";
        case ResultType::Blocked: return "Blocked";
        case ResultType::Skipped: return "Skipped";
        case ResultType::Acceptable: return "Acceptable";
        case ResultType::Failed: return "Failed";
        default: return "Unknown";
    }
}

ImVec4 TestViewer::get_test_result_color(ResultType result) const 
{
    switch (result) {
        case ResultType::Passed: return ImVec4(0.0f, 0.8f, 0.0f, 1.0f); // Green
        case ResultType::Failed: return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        case ResultType::Blocked: return ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // Orange
        case ResultType::Skipped: return ImVec4(0.7f, 0.7f, 0.7f, 1.0f); // Gray
        case ResultType::Acceptable: return ImVec4(1.0f, 0.8f, 0.0f, 1.0f); // Yellow
        default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
    }
}

}
