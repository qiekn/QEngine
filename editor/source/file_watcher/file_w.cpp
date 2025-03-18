#include "file_watcher/file_w.h"

FileWatcher::FileWatcher(const std::string& path_to_watch, std::chrono::duration<int, std::milli> polling_interval)
    : m_path_to_watch(path_to_watch), m_polling_interval(polling_interval), m_running(false) {
    for(auto& file : fs::recursive_directory_iterator(m_path_to_watch)) {
        if (fs::is_regular_file(file)) {
            m_paths[file.path().string()] = fs::last_write_time(file);
        }
    }
}


FileWatcher::~FileWatcher() {
    stop();
}

void FileWatcher::add_callback(const std::vector<std::string>& extensions, Callback callback) {
    for (const auto& ext : extensions) {
        m_callbacks[ext].push_back(callback);
    }
}

void FileWatcher::add_callback(Callback callback) {
    m_general_callbacks.push_back(callback);
}

void FileWatcher::start() {
    m_running = true;
    m_watch_thread = std::thread(&FileWatcher::watch_loop, this);
}

void FileWatcher::stop() {
    m_running = false;
    if (m_watch_thread.joinable()) {
        m_watch_thread.join();
    }
}

void FileWatcher::watch_loop() {
    while (m_running) {
        auto it = m_paths.begin();
        while (it != m_paths.end()) {
            if (!fs::exists(it->first)) {
                notify_callbacks(it->first, "deleted");
                it = m_paths.erase(it);
            } else {
                it++;
            }
        }
        for (auto& file : fs::recursive_directory_iterator(m_path_to_watch)) {
            if (fs::is_regular_file(file)) {
                auto current_file_last_write_time = fs::last_write_time(file);
                std::string path = file.path().string();
                if (m_paths.find(path) != m_paths.end()) {
                    if (m_paths[path] != current_file_last_write_time) {
                        m_paths[path] = current_file_last_write_time;
                        notify_callbacks(path, "modified");
                    }
                }
                else {
                    m_paths[path] = current_file_last_write_time;
                    notify_callbacks(path, "created");
                }
            }
        }
        std::this_thread::sleep_for(m_polling_interval);
    }
}

void FileWatcher::notify_callbacks(const std::string& path, const std::string& status) {
    fs::path filepath(path);
    std::string extension = filepath.extension().string();
    
    if (m_callbacks.find(extension) != m_callbacks.end()) {
        for (const auto& callback : m_callbacks[extension]) {
            callback(filepath, status);
        }
    }
    
    for (const auto& callback : m_general_callbacks) {
        callback(filepath, status);
    }
}
