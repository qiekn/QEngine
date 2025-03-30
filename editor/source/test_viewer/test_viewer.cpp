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

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.65f, 0.65f, 0.75f, 0.2));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 5.0f);

    ImGui::BeginChild("test_summary", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 4), true);
    render_toolbar();
    render_test_summary();
    ImGui::EndChild();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.65f, 0.65f, 0.75f, 0.4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    ImGui::Separator();

    ImGui::BeginChild("test_details_panel", ImVec2(0, 0), true);
    for (int i = 0; i < test_plan.test_cases.size(); i++) {
        render_single_test_case(i);

        if (i < test_plan.test_cases.size() - 1) {
            ImGui::Separator();
        }
    }
    ImGui::EndChild();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}


void TestViewer::render_toolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));

    if (ImGui::Button("Save Results")) {
        save_results(test_plan.name + "_results.csv");
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset All Tests")) {
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
            reset_tests();
            update_test_statistics();
        } else {
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Hold Ctrl and click to reset all tests");
            }
        }
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    static char filter[64] = "";
    ImGui::InputTextWithHint("##filter", "Filter tests...", filter, sizeof(filter));

    ImGui::SameLine(ImGui::GetWindowWidth() - 120);
    ImGui::Text("Plan: %s", test_plan.name.c_str());

    ImGui::PopStyleVar();
}

void TestViewer::render_single_test_case(int index)
{
    auto& test = test_plan.test_cases[index];

    ImGui::PushID(index);

    ImVec4 headerColor = ImVec4(0.25f, 0.25f, 0.27f, 0.8f); 

    if (test.is_executed()) {
      headerColor = get_test_result_color(test.actual_result.type);

      headerColor.x *= 0.7f;
      headerColor.y *= 0.7f;
      headerColor.z *= 0.7f;
      headerColor.w = 0.6f;
    }

    ImVec2 headerStart = ImGui::GetCursorScreenPos();
    ImVec2 headerEnd = ImVec2(
        headerStart.x + ImGui::GetContentRegionAvail().x,
        headerStart.y + ImGui::GetFrameHeight() + 4
    );

    ImGui::GetWindowDrawList()->AddRectFilled(
        headerStart, headerEnd, ImGui::ColorConvertFloat4ToU32(headerColor)
    );

    ImGui::Dummy(ImVec2(0, 2));
    ImGui::Indent(5);

    std::string header = "Test " + std::to_string(index + 1);

    if (expanded_tests.find(index) == expanded_tests.end()) {
        expanded_tests[index] = false;
    }

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    bool is_open = expanded_tests[index];
    bool clicked = false;

    ImGui::Text(is_open ? "v" : ">");
    ImGui::SameLine();

    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerColor); 
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerColor); 
    ImGui::PushStyleColor(ImGuiCol_Header, headerColor);      

    if (ImGui::Selectable(header.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
        expanded_tests[index] = !is_open;
        clicked = true;
    }

    ImGui::PopStyleColor(3); 


    ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
    ImGui::Text("Status: ");
    ImGui::SameLine();

    ImGui::TextColored(get_test_result_color(test.actual_result.type),
                         "%s", get_test_result_string(test.actual_result.type).c_str());

    ImGui::Unindent(5);
    ImGui::Dummy(ImVec2(0, 2));

    if (expanded_tests[index]) {
        ImVec2 contentStart = ImGui::GetCursorScreenPos();
        ImVec2 contentEnd = ImVec2(
            contentStart.x + ImGui::GetContentRegionAvail().x,
            contentStart.y + ImGui::GetFrameHeightWithSpacing() * 9 + 16 
        );

        ImGui::GetWindowDrawList()->AddRectFilled(
            contentStart, contentEnd,
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.18f, 0.18f, 0.2f, 0.8f))
        );

        ImGui::GetWindowDrawList()->AddRect(
            contentStart, contentEnd,
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.3f, 0.3f, 0.35f, 0.5f))
        );

        ImGui::Dummy(ImVec2(0, 8));
        ImGui::Indent(12);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.9f, 1.0f, 1.0f));
        ImGui::TextUnformatted("Action:");
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.17f, 0.8f));
        ImGui::BeginChild(("action_text_" + std::to_string(index)).c_str(),
                        ImVec2(ImGui::GetContentRegionAvail().x - 8, ImGui::GetTextLineHeightWithSpacing() * 3),
                        true);
        ImGui::TextWrapped("%s", test.action.value.c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0, 8));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.9f, 1.0f, 1.0f));
        ImGui::TextUnformatted("Expected Result:");
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.17f, 0.8f));
        ImGui::BeginChild(("expected_result_text_" + std::to_string(index)).c_str(),
                        ImVec2(ImGui::GetContentRegionAvail().x - 8, ImGui::GetTextLineHeightWithSpacing() * 3),
                        true);
        ImGui::TextWrapped("%s", test.expected_result.value.c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0, 8));

        bool show_actual_result = ImGui::CollapsingHeader("Actual Result", ImGuiTreeNodeFlags_None);

        if (show_actual_result) {
            if (actual_result_buffers.find(index) == actual_result_buffers.end()) {
                actual_result_buffers[index] = std::vector<char>(4096, 0);
                if (!test.actual_result.value.empty()) {
                    strncpy(actual_result_buffers[index].data(), test.actual_result.value.c_str(),
                            actual_result_buffers[index].size() - 1);
                }
            }

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.15f, 0.8f));
            std::string buffer_id = "##actual_result_" + std::to_string(index);
            if (ImGui::InputTextMultiline(buffer_id.c_str(),
                                      actual_result_buffers[index].data(),
                                      actual_result_buffers[index].size(),
                                      ImVec2(ImGui::GetContentRegionAvail().x - 8, ImGui::GetTextLineHeightWithSpacing() * 4),
                                      ImGuiInputTextFlags_AllowTabInput)) {
                test.actual_result.value = actual_result_buffers[index].data();
            }
            ImGui::PopStyleColor();
        }

        ImGui::Dummy(ImVec2(0, 2));

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

        float available_width = ImGui::GetContentRegionAvail().x - 12;
        int buttons_per_row = 3; 
        float button_width = (available_width / buttons_per_row) - 8;

        std::unordered_map<ResultType, ImVec4> resultColors;
        resultColors[ResultType::Todo] = ImVec4(0.4f, 0.4f, 0.7f, 0.7f);
        resultColors[ResultType::Passed] = ImVec4(0.2f, 0.7f, 0.3f, 0.7f);
        resultColors[ResultType::Failed] = ImVec4(0.7f, 0.2f, 0.2f, 0.7f);
        resultColors[ResultType::Blocked] = ImVec4(0.7f, 0.4f, 0.1f, 0.7f);
        resultColors[ResultType::Skipped] = ImVec4(0.4f, 0.4f, 0.4f, 0.7f);
        resultColors[ResultType::Acceptable] = ImVec4(0.7f, 0.6f, 0.1f, 0.7f);

        int current_button = 0;
        for (int i = 1; i < static_cast<int>(ResultType::Length); i++) {
            const ResultType result_type = static_cast<ResultType>(i);
            const std::string& name = get_test_result_string(result_type);

            if (current_button > 0 && current_button % buttons_per_row == 0) {
                ImGui::NewLine();
            } else if (current_button > 0) {
                ImGui::SameLine();
            }

            current_button++;

            bool is_current = test.is_executed() && test.actual_result.type == result_type;
            if (is_current) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(
                    resultColors[result_type].x * 1.2f,
                    resultColors[result_type].y * 1.2f,
                    resultColors[result_type].z * 1.2f,
                    0.9f));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, resultColors[result_type]);
            }

            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(
                std::min(resultColors[result_type].x * 1.3f, 1.0f),
                std::min(resultColors[result_type].y * 1.3f, 1.0f),
                std::min(resultColors[result_type].z * 1.3f, 1.0f),
                0.8f));

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

            if(ImGui::Button(name.c_str(), ImVec2(button_width, 0))) {
                //test.is_executed = true;
                test.actual_result.type = result_type;
                update_test_statistics();
            }

            ImGui::PopStyleColor(3);
        }

        ImGui::PopStyleVar(); 

        ImGui::Unindent(12);
        ImGui::Dummy(ImVec2(0, 8));
    }

    ImGui::Dummy(ImVec2(0, 4));

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
        if (test.is_executed()) {
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

void TestViewer::render_test_list() {
    ImGui::BeginChild("test_list", ImVec2(0, 0), true);

    if (test_plan.test_cases.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No test cases available.");
    } else {
        for (int i = 0; i < test_plan.test_cases.size(); i++) {
            render_single_test_case(i);

            if (i < test_plan.test_cases.size() - 1) {
                ImGui::Separator();
            }
        }
    }

    ImGui::EndChild();
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
    
    if (test.is_executed()) {
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
            //current_case.is_executed = true;
            current_case.actual_result.type = static_cast<ResultType>(i);
        }
    }
    
    static char notes[4096] = "";
    ImGui::Separator();
    ImGui::Text("Notes:");
    ImGui::InputTextMultiline("##notes", notes, sizeof(notes), ImVec2(-1, ImGui::GetContentRegionAvail().y), ImGuiInputTextFlags_AllowTabInput);
}

void TestViewer::render_test_summary() {
    ImGui::BeginChild("test_summary", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 3.5f), true);

    float completion = (float)tests_executed / test_plan.test_cases.size();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Progress:");
    ImGui::SameLine();

    float barWidth = ImGui::GetContentRegionAvail().x;
    ImVec2 cursorPos = ImGui::GetCursorPos();

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3f, 0.5f, 0.8f, 0.8f));
    ImGui::ProgressBar(completion, ImVec2(barWidth, 14.0f), "");
    ImGui::PopStyleColor(2);

    ImVec2 textPos = cursorPos;
    textPos.x += barWidth * 0.5f - 20;
    textPos.y += 0;
    ImGui::SetCursorPos(textPos);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%.0f%%", completion * 100);

    ImGui::Columns(6, "test_stats_columns", false);
    ImGui::SetColumnWidth(0, barWidth * 0.16f);
    ImGui::SetColumnWidth(1, barWidth * 0.17f);
    ImGui::SetColumnWidth(2, barWidth * 0.17f);
    ImGui::SetColumnWidth(3, barWidth * 0.17f);
    ImGui::SetColumnWidth(4, barWidth * 0.17f);
    ImGui::SetColumnWidth(5, barWidth * 0.16f);

    ImGui::Text("Total: %d", (int)test_plan.test_cases.size());
    ImGui::NextColumn();

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
        float pass_percentage = (float)(tests_passed + tests_acceptable) / tests_executed * 100.0f;
        ImGui::Text("Pass Rate: %.1f%%", pass_percentage);
    }

    ImGui::EndChild();
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

    for(auto& test_case : test_plan.test_cases) {
        test_case.actual_result.type = ResultType::Todo;
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
        
        file << "Action,Expected Result,Actual Result\n";
        
        for (const auto& test : test_plan.test_cases) {
            auto escape_csv_field = [](const std::string& field) -> std::string {
                if (field.find(',') != std::string::npos || field.find('"') != std::string::npos || field.find('\n') != std::string::npos) {
                    std::string escaped = field;
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
            
            file << get_test_result_string(test.actual_result.type) << "," << escape_csv_field(test.actual_result.value) << ",";
            
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
        test.actual_result.type = ResultType::Todo;
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
