#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <imgui.h>

namespace Test {

enum class Status { 
    NONE = 0,
    TODO,
    PASSED,
    BLOCKED,
    SKIPPED,
    ACCEPTABLE,
    FAILED,
    LENGTH,
};

struct TestStep {
    std::string id;
    std::string action;
    std::string data;
    std::string result;
    Status status = Status::TODO;
    std::string actual_result;
    std::string comment;
    std::vector<std::string> defects;
    std::vector<std::string> historical_defects;
};

struct TestRun {
    std::string id;
    std::string test_key;
    std::string test_summary;
    std::vector<TestStep> steps;
    Status status = Status::TODO;
    bool is_executed() const {
        if (status != Status::TODO) return true;
        for (const auto& step : steps) {
            if (step.status != Status::TODO) return true;
        }
        return false;
    }
};

struct TestExecution { 
    std::string key;
    std::string summary;
    std::string description;
    std::vector<TestRun> test_runs;
};

struct TestStatistics {
    int executed_runs = 0;
    int passed_runs = 0;
    int failed_runs = 0;
    int blocked_runs = 0;
    int skipped_runs = 0;
    int acceptable_runs = 0;
};

class TestViewer {
public:
    TestViewer();
    ~TestViewer();

    void render();
    void reset_all_tests();
    
private:
    void create_mockup_data();
    void render_toolbar();
    void render_summary();
    void render_test_runs();
    void render_test_run(int index);
    void render_test_step(TestRun& run, int step_idx);
    
    template<typename T>
    void render_status_buttons(T& item, float button_width);
    
    void update_statistics();

    TestExecution m_test_execution;
    std::unordered_map<std::string, std::vector<char>> m_actual_result_buffers;
    TestStatistics m_stats;
};

} 
