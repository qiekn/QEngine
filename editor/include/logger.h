#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <mutex>
#include <functional>

enum class LogLevel {
    TRACE,  
    INFO,
    WARNING,
    ERROR
};

class Logger;

class LogStream {
public:
    LogStream(Logger& logger, LogLevel level);
    ~LogStream();
    
    template <typename T>
    LogStream& operator<<(const T& value) {
        m_stream << value;
        return *this;
    }
    
    typedef std::ostream& (*StreamManipulator)(std::ostream&);
    LogStream& operator<<(StreamManipulator manip) {
        manip(m_stream);
        return *this;
    }

private:
    std::ostringstream m_stream;
    Logger& m_logger;
    LogLevel m_level;
};

class Logger {
public:
    static Logger& get() {
        static Logger instance;
        return instance;
    }
    
    LogStream trace() { return LogStream(*this, LogLevel::TRACE); }   
    LogStream info() { return LogStream(*this, LogLevel::INFO); }
    LogStream warning() { return LogStream(*this, LogLevel::WARNING); }
    LogStream error() { return LogStream(*this, LogLevel::ERROR); }
    
    void log(LogLevel level, const std::string& message);
    
    const std::vector<std::pair<LogLevel, std::string>>& get_log_messages() const {
        return m_logs;
    }
    
    void register_callback(std::function<void(LogLevel, const std::string&)> callback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callbacks.push_back(callback);
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_logs.clear();
    }
    
    void set_min_log_level(LogLevel level) {
        m_min_log_level = level;
    }
    
    LogLevel get_min_log_level() const {
        return m_min_log_level;
    }
    
    static std::string level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "[TRACE]";  
            case LogLevel::INFO: return "[INFO]";
            case LogLevel::WARNING: return "[WARNING]";
            case LogLevel::ERROR: return "[ERROR]";
            default: return "[UNKNOWN]";
        }
    }

private:
    Logger() : m_min_log_level(LogLevel::TRACE) {}  
    ~Logger() = default;
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::vector<std::pair<LogLevel, std::string>> m_logs;
    std::vector<std::function<void(LogLevel, const std::string&)>> m_callbacks;
    std::mutex m_mutex;
    LogLevel m_min_log_level;
};

#define log_trace() Logger::get().trace() << "[EDITOR] "
#define log_info() Logger::get().info() << "[EDITOR] "
#define log_warning() Logger::get().warning() << "[EDITOR] "
#define log_error() Logger::get().error()  << "[EDITOR] "
