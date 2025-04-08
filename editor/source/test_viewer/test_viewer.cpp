#include "test_viewer/test_viewer.h"
#include "imgui.h"
#include "logger.h"

namespace Test {

std::string get_status_string(Status status) {
    switch (status) {
        case Status::NONE: return "None";
        case Status::TODO: return "Todo";
        case Status::PASSED: return "Passed";
        case Status::BLOCKED: return "Blocked";
        case Status::SKIPPED: return "Skipped";
        case Status::ACCEPTABLE: return "Acceptable";
        case Status::FAILED: return "Failed";
        default: return "Unknown";
    }
}

ImVec4 get_status_color(Status status) {
    switch (status) {
        case Status::PASSED: return ImVec4(0.0f, 0.8f, 0.0f, 1.0f); // Green
        case Status::FAILED: return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        case Status::BLOCKED: return ImVec4(1.0f, 0.5f, 0.0f, 1.0f); // Orange
        case Status::SKIPPED: return ImVec4(0.7f, 0.7f, 0.7f, 1.0f); // Gray
        case Status::ACCEPTABLE: return ImVec4(1.0f, 0.8f, 0.0f, 1.0f); // Yellow
        default: return ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray for TODO/NONE
    }
}

// Get a lighter version of the status color for backgrounds to ensure text readability
ImVec4 get_status_bg_color(Status status) {
    ImVec4 color = get_status_color(status);
    // Make the color lighter and more transparent for better text readability
    return ImVec4(
        color.x * 0.5f + 0.5f, // Lighten
        color.y * 0.5f + 0.5f,
        color.z * 0.5f + 0.5f,
        0.3f // More transparent
    );
}

TestViewer::TestViewer() {
    create_mockup_data();
}

TestViewer::~TestViewer() {}

void TestViewer::create_mockup_data() {
    m_test_execution.key = "XSP1-123";
    m_test_execution.summary = "Browser Compatibility Test Plan";
    m_test_execution.description = "Testing the application across different browsers and platforms";
    
    TestRun run1;
    run1.id = "RUN-001";
    run1.test_key = "TEST-001";
    run1.test_summary = "Verify main menu navigation";
    
    TestStep step1;
    step1.id = "STEP-001";
    step1.action = "Launch the application";
    step1.result = "Application opens successfully with the main menu visible";
    run1.steps.push_back(step1);
    
    TestStep step2;
    step2.id = "STEP-002";
    step2.action = "Click on each menu item in sequence";
    step2.result = "Each menu item correctly navigates to the corresponding screen";
    run1.steps.push_back(step2);
    
    m_test_execution.test_runs.push_back(run1);
    
    TestRun run2;
    run2.id = "RUN-002";
    run2.test_key = "TEST-002";
    run2.test_summary = "Test file saving functionality";
    
    TestStep step3;
    step3.id = "STEP-003";
    step3.action = "Create a new document and add content";
    step3.result = "Document is created and content is displayed correctly";
    run2.steps.push_back(step3);
    
    TestStep step4;
    step4.id = "STEP-004";
    step4.action = "Save the document using File > Save";
    step4.result = "Document is saved successfully with confirmation message";
    run2.steps.push_back(step4);
    
    TestStep step5;
    step5.id = "STEP-005";
    step5.action = "Close and reopen the application, then open the saved document";
    step5.result = "Document opens with all content intact";
    run2.steps.push_back(step5);
    
    m_test_execution.test_runs.push_back(run2);
    
    TestRun run3;
    run3.id = "RUN-003";
    run3.test_key = "TEST-003";
    run3.test_summary = "Verify entity creation and deletion";
    
    TestStep step6;
    step6.id = "STEP-006";
    step6.action = "Create a new entity using the Create button";
    step6.result = "Entity is created and appears in the hierarchy";
    run3.steps.push_back(step6);
    
    TestStep step7;
    step7.id = "STEP-007";
    step7.action = "Add components to the entity";
    step7.result = "Components are successfully added and visible in the inspector";
    run3.steps.push_back(step7);
    
    TestStep step8;
    step8.id = "STEP-008";
    step8.action = "Delete the entity";
    step8.result = "Entity is removed from the hierarchy";
    run3.steps.push_back(step8);
    
    m_test_execution.test_runs.push_back(run3);
    
    update_statistics();
}

void TestViewer::render() {
    // Display test execution info
    ImGui::Text("Test Execution: %s - %s", m_test_execution.key.c_str(), m_test_execution.summary.c_str());
    
    if (!m_test_execution.description.empty()) {
        ImGui::TextWrapped("%s", m_test_execution.description.c_str());
    }
    
    ImGui::Separator();
    
    render_toolbar();
    
    ImGui::BeginChild("test_summary", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 4), true);
    render_summary();
    ImGui::EndChild();
    
    ImGui::Separator();
    
    ImGui::BeginChild("test_details", ImVec2(0, 0), true);
    render_test_runs();
    ImGui::EndChild();
}

void TestViewer::render_toolbar() {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 3));
    
    if (ImGui::Button("Save Results")) {
        log_info() << "Saving test results..." << std::endl;
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Reset All")) {
        reset_all_tests();
        update_statistics();
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Export to Jira")) {
        log_info() << "Exporting to Jira..." << std::endl;
    }
    
    ImGui::PopStyleVar();
}

void TestViewer::render_summary() {
    // Calculate progress
    float completion = m_test_execution.test_runs.empty() ? 0.0f : 
        (float)m_stats.executed_runs / m_test_execution.test_runs.size();
    
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
    ImGui::SetCursorPos(textPos);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%.0f%%", completion * 100);
    
    // Display statistics in columns
    ImGui::Columns(6, "test_stats_columns", false);
    
    ImGui::Text("Total: %d", (int)m_test_execution.test_runs.size());
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), "Passed: %d", m_stats.passed_runs);
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Failed: %d", m_stats.failed_runs);
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Blocked: %d", m_stats.blocked_runs);
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Skipped: %d", m_stats.skipped_runs);
    ImGui::NextColumn();
    
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Acceptable: %d", m_stats.acceptable_runs);
    ImGui::NextColumn();
    
    ImGui::Columns(1);
    
    if (m_stats.executed_runs > 0) {
        float pass_percentage = (float)(m_stats.passed_runs + m_stats.acceptable_runs) / m_stats.executed_runs * 100.0f;
        ImGui::Text("Pass Rate: %.1f%%", pass_percentage);
    }
}

void TestViewer::render_test_runs() {
    for (int i = 0; i < m_test_execution.test_runs.size(); i++) {
        render_test_run(i);
        
        if (i < m_test_execution.test_runs.size() - 1) {
            ImGui::Separator();
        }
    }
}

void TestViewer::render_test_run(int index) {
    TestRun& run = m_test_execution.test_runs[index];

    ImGui::PushID(index);

    // Create a header with a better background color for readability
    ImGui::PushStyleColor(ImGuiCol_Header, get_status_bg_color(run.status));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, get_status_bg_color(run.status));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, get_status_bg_color(run.status));

    bool is_open = ImGui::CollapsingHeader((run.test_key + ": " + run.test_summary).c_str(),
                                         ImGuiTreeNodeFlags_DefaultOpen);

    ImGui::PopStyleColor(3);

    // Show status indicator
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 100);
    ImGui::TextColored(get_status_color(run.status), "%s", get_status_string(run.status).c_str());

    if (is_open) {
        ImGui::Indent(12);

        // Render steps
        for (int step_idx = 0; step_idx < run.steps.size(); step_idx++) {
            render_test_step(run, step_idx);

            if (step_idx < run.steps.size() - 1) {
                ImGui::Separator();
            }
        }

        ImGui::Dummy(ImVec2(0, 8));

        // Test run status buttons
        ImGui::Text("Set overall test status:");

        float available_width = ImGui::GetContentRegionAvail().x - 12;
        int buttons_per_row = 5;
        float button_width = (available_width / buttons_per_row) - 4;

        render_status_buttons(run, button_width);

        ImGui::Unindent(12);
    }

    ImGui::Dummy(ImVec2(0, 4));
    ImGui::PopID();
}

void TestViewer::render_test_step(TestRun& run, int step_idx) {
    TestStep& step = run.steps[step_idx];
    
    ImGui::PushID(step_idx);
    
    // Create a similar header style to the test runs but with a lighter color
    ImGui::PushStyleColor(ImGuiCol_Header, get_status_bg_color(step.status));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, get_status_bg_color(step.status));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, get_status_bg_color(step.status));
    
    bool is_open = ImGui::CollapsingHeader(("Step " + std::to_string(step_idx + 1)).c_str());
    
    ImGui::PopStyleColor(3);
    
    // Always show the status, not just when collapsed
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 100);
    ImGui::TextColored(get_status_color(step.status), "%s", get_status_string(step.status).c_str());
    
    if (is_open) {
        ImGui::Indent(8);
        
        // Action
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.9f, 1.0f, 1.0f));
        ImGui::TextUnformatted("Action:");
        ImGui::PopStyleColor();
        
        ImGui::BeginChild(("action_text_" + step.id).c_str(),
                        ImVec2(ImGui::GetContentRegionAvail().x - 8, ImGui::GetTextLineHeightWithSpacing() * 3),
                        true);
        ImGui::TextWrapped("%s", step.action.c_str());
        ImGui::EndChild();
        
        // Expected result
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.9f, 1.0f, 1.0f));
        ImGui::TextUnformatted("Expected Result:");
        ImGui::PopStyleColor();
        
        ImGui::BeginChild(("expected_result_text_" + step.id).c_str(),
                        ImVec2(ImGui::GetContentRegionAvail().x - 8, ImGui::GetTextLineHeightWithSpacing() * 3),
                        true);
        ImGui::TextWrapped("%s", step.result.c_str());
        ImGui::EndChild();
        
        // Actual result
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.9f, 1.0f, 1.0f));
        ImGui::TextUnformatted("Actual Result:");
        ImGui::PopStyleColor();
        
        // Get or create the buffer for this step
        auto& buffer = m_actual_result_buffers[step.id];
        if (buffer.empty()) {
            buffer.resize(2048, 0);
            if (!step.actual_result.empty()) {
                strncpy(buffer.data(), step.actual_result.c_str(), buffer.size() - 1);
            }
        }
        
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.15f, 0.8f));
        
        if (ImGui::InputTextMultiline(("##actual_result_" + step.id).c_str(),
                                  buffer.data(),
                                  buffer.size(),
                                  ImVec2(ImGui::GetContentRegionAvail().x - 8, ImGui::GetTextLineHeightWithSpacing() * 3),
                                  ImGuiInputTextFlags_AllowTabInput)) {
            step.actual_result = buffer.data();
        }
        
        ImGui::PopStyleColor();
        
        // Step status buttons
        ImGui::Dummy(ImVec2(0, 4));
        ImGui::Text("Step Result:");
        
        float available_width = ImGui::GetContentRegionAvail().x - 12;
        int buttons_per_row = 5;
        float button_width = (available_width / buttons_per_row) - 4;
        
        render_status_buttons(step, button_width);
        
        ImGui::Unindent(8);
    }
    
    ImGui::PopID();
}

template<typename T>
void TestViewer::render_status_buttons(T& item, float button_width) {
    static std::unordered_map<Status, ImVec4> status_colors = {
        {Status::TODO, ImVec4(0.4f, 0.4f, 0.7f, 0.7f)},
        {Status::PASSED, ImVec4(0.2f, 0.7f, 0.3f, 0.7f)},
        {Status::FAILED, ImVec4(0.7f, 0.2f, 0.2f, 0.7f)},
        {Status::BLOCKED, ImVec4(0.7f, 0.4f, 0.1f, 0.7f)},
        {Status::SKIPPED, ImVec4(0.4f, 0.4f, 0.4f, 0.7f)},
        {Status::ACCEPTABLE, ImVec4(0.7f, 0.6f, 0.1f, 0.7f)}
    };
    
    int current_button = 0;
    for (int i = 1; i < static_cast<int>(Status::LENGTH); i++) {
        const Status status = static_cast<Status>(i);
        const std::string& name = get_status_string(status);
        
        if (current_button > 0 && current_button % 3 == 0) {
            ImGui::NewLine();
        } else if (current_button > 0) {
            ImGui::SameLine();
        }
        
        current_button++;
        
        bool is_current = item.status == status;
        
        if (is_current) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(
                status_colors[status].x * 1.2f,
                status_colors[status].y * 1.2f,
                status_colors[status].z * 1.2f,
                0.9f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, status_colors[status]);
        }
        
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(
            std::min(status_colors[status].x * 1.3f, 1.0f),
            std::min(status_colors[status].y * 1.3f, 1.0f),
            std::min(status_colors[status].z * 1.3f, 1.0f),
            0.8f));
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        if (ImGui::Button(name.c_str(), ImVec2(button_width, 0))) {
            item.status = status;
            update_statistics();
        }
        
        ImGui::PopStyleColor(3);
    }
}

void TestViewer::update_statistics() {
    m_stats = {};
    
    for (const auto& run : m_test_execution.test_runs) {
        if (run.is_executed()) {
            m_stats.executed_runs++;
            
            switch (run.status) {
                case Status::PASSED: m_stats.passed_runs++; break;
                case Status::FAILED: m_stats.failed_runs++; break;
                case Status::BLOCKED: m_stats.blocked_runs++; break;
                case Status::SKIPPED: m_stats.skipped_runs++; break;
                case Status::ACCEPTABLE: m_stats.acceptable_runs++; break;
                default: break;
            }
        }
    }
}

void TestViewer::reset_all_tests() {
    for (auto& run : m_test_execution.test_runs) {
        run.status = Status::TODO;
        
        for (auto& step : run.steps) {
            step.status = Status::TODO;
            step.actual_result.clear();
            step.comment.clear();
            step.defects.clear();
        }
    }
    
    m_actual_result_buffers.clear();
}

} // namespace Test
