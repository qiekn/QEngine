#include "file_watcher/file_watcher.h"

FileWatcher::FileWatcher(const std::string& path_to_watch, std::chrono::duration<int, std::milli> polling_interval)
    : path_to_watch_(path_to_watch), polling_interval_(polling_interval), running_(false) {
    for(auto& file : fs::recursive_directory_iterator(path_to_watch_)) {
        if (fs::is_regular_file(file)) {
            paths_[file.path().string()] = fs::last_write_time(file);
        }
    }
}

FileWatcher::~FileWatcher() {
    stop();
}

void FileWatcher::addCallback(const std::vector<std::string>& extensions, Callback callback) {
    for (const auto& ext : extensions) {
        callbacks_[ext].push_back(callback);
    }
}

void FileWatcher::addCallback(Callback callback) {
    general_callbacks_.push_back(callback);
}

void FileWatcher::start() {
    running_ = true;
    watch_thread_ = std::thread(&FileWatcher::watchLoop, this);
}

void FileWatcher::stop() {
    running_ = false;
    if (watch_thread_.joinable()) {
        watch_thread_.join();
    }
}

void FileWatcher::watchLoop() {
    while (running_) {
        auto it = paths_.begin();
        while (it != paths_.end()) {
            if (!fs::exists(it->first)) {
                notifyCallbacks(it->first, "deleted");
                it = paths_.erase(it);
            } else {
                it++;
            }
        }

        for (auto& file : fs::recursive_directory_iterator(path_to_watch_)) {
            if (fs::is_regular_file(file)) {
                auto current_file_last_write_time = fs::last_write_time(file);
                std::string path = file.path().string();

                if (paths_.find(path) != paths_.end()) {
                    if (paths_[path] != current_file_last_write_time) {
                        paths_[path] = current_file_last_write_time;
                        notifyCallbacks(path, "modified");
                    }
                }
                else {
                    paths_[path] = current_file_last_write_time;
                    notifyCallbacks(path, "created");
                }
            }
        }

        std::this_thread::sleep_for(polling_interval_);
    }
}

void FileWatcher::notifyCallbacks(const std::string& path, const std::string& status) {
    fs::path filepath(path);
    std::string extension = filepath.extension().string();
    
    if (callbacks_.find(extension) != callbacks_.end()) {
        for (const auto& callback : callbacks_[extension]) {
            callback(filepath, status);
        }
    }
    
    for (const auto& callback : general_callbacks_) {
        callback(filepath, status);
    }
}
