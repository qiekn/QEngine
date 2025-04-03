#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <functional>
#include <future>

#include <imgui.h>

namespace Test {

enum class ResultType { 
    None = 0,
    Todo,
    Passed,
    Blocked,
    Skipped,
    Acceptable,
    Failed,
    Length,
};

struct Action { 
    std::string value;
};

struct ExpectedResult { 
    std::string value;
};

struct ActualResult {
    std::string value;
    ResultType type = ResultType::Todo;
};

struct TestCase { 
    Action action;
    ExpectedResult expected_result;
    ActualResult actual_result;
    inline bool is_executed() const { return actual_result.type != ResultType::Todo; }
};

struct TestPlan { 
    std::vector<TestCase> test_cases;
    std::string name;
    std::string description;
};

class TestViewer { 
public:
    TestViewer();
    ~TestViewer();

    void render();
    void load_test_file(const std::string& file_path);
    void save_results(const std::string& file_path);
    void reset_tests();
    void fetch_manual_tests();
    std::vector<std::string> split_csv_line(const std::string&);
    
    inline bool is_test_loaded() const { return !test_plan.test_cases.empty(); }
    inline const TestPlan& get_test_plan() const { return test_plan; }
    
private:
    void parse_csv(const std::string& content);
    TestCase process_csv_line(const std::string& line);
    void render_test_list();
    void render_test_summary();
    void render_toolbar();
    void check_fetch_status();
    
    std::string get_test_result_string(ResultType result_type) const;
    ImVec4 get_test_result_color(ResultType result_type) const;
    
    TestPlan test_plan;
    int selected_test_index = -1;
    
    char filter_buffer[256] = "";
    
    int tests_passed = 0;
    int tests_failed = 0;
    int tests_blocked = 0;
    int tests_skipped = 0;
    int tests_acceptable = 0;
    int tests_executed = 0;

    bool m_open = false;

    void render_single_test_case(int index);
    void update_test_statistics();
    std::unordered_map<int, std::vector<char>> actual_result_buffers;

    std::unordered_map<int, bool> expanded_tests;
    
    std::future<void> m_fetch_future;
    bool m_is_fetching = false;
    bool m_fetch_succeeded = false;
    std::string m_fetch_message;


    std::future<void> m_xray_future;
    bool m_is_sending_to_xray = false;
    bool m_xray_succeeded = false;
    std::string m_xray_message;

    void send_to_xray();
    void check_xray_status();
};

}
