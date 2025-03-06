#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

class FileWatcher {
public:
    using Callback = std::function<void(const fs::path&, const std::string&)>;

    FileWatcher(const std::string& path_to_watch, 
                std::chrono::duration<int, std::milli> polling_interval = std::chrono::milliseconds(1000));
    ~FileWatcher();

    void addCallback(const std::vector<std::string>& extensions, Callback callback);
    void addCallback(Callback callback);
    void start();
    void stop();

private:
    std::string path_to_watch_;
    std::chrono::duration<int, std::milli> polling_interval_;
    std::unordered_map<std::string, fs::file_time_type> paths_;
    std::map<std::string, std::vector<Callback>> callbacks_;
    std::vector<Callback> general_callbacks_;
    bool running_;
    std::thread watch_thread_;

    void watchLoop();
    void notifyCallbacks(const std::string& path, const std::string& status);
};
