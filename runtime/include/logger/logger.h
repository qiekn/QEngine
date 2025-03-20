#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <functional>

enum class LogLevel {
    TRACE,  
    INFO,
    WARNING,
    ERROR
};

class RemoteLogger;

// Stream class to handle the log << syntax
class RemoteLogStream {
public:
    RemoteLogStream(RemoteLogger& logger, LogLevel level);
    ~RemoteLogStream();
    
    template <typename T>
    RemoteLogStream& operator<<(const T& value) {
        m_stream << value;
        return *this;
    }
    
    typedef std::ostream& (*StreamManipulator)(std::ostream&);
    RemoteLogStream& operator<<(StreamManipulator manip) {
        manip(m_stream);
        return *this;
    }

private:
    std::ostringstream m_stream;
    RemoteLogger& m_logger;
    LogLevel m_level;
};

// Main logger class that sends logs to the editor
class RemoteLogger {
public:
    static RemoteLogger& get() {
        static RemoteLogger instance;
        return instance;
    }
    
    RemoteLogStream trace() { return RemoteLogStream(*this, LogLevel::TRACE); }   
    RemoteLogStream info() { return RemoteLogStream(*this, LogLevel::INFO); }
    RemoteLogStream warning() { return RemoteLogStream(*this, LogLevel::WARNING); }
    RemoteLogStream error() { return RemoteLogStream(*this, LogLevel::ERROR); }
    
    // Called by RemoteLogStream destructor to send the log message
    void log(LogLevel level, const std::string& message);
    
    // Set a function that will be called to send messages to the editor
    void set_send_function(std::function<bool(const std::string&)> send_func) {
        m_send_func = send_func;
    }
    
    void set_min_log_level(LogLevel level) {
        m_min_log_level = level;
    }
    
    LogLevel get_min_log_level() const {
        return m_min_log_level;
    }
    
    static std::string level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";  
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }

private:
    RemoteLogger() : m_min_log_level(LogLevel::TRACE) {}
    ~RemoteLogger() = default;
    
    RemoteLogger(const RemoteLogger&) = delete;
    RemoteLogger& operator=(const RemoteLogger&) = delete;
    
    LogLevel m_min_log_level;
    std::function<bool(const std::string&)> m_send_func;
};

// Define macros for easy logging
#define remote_log_trace() RemoteLogger::get().trace()  
#define remote_log_info() RemoteLogger::get().info()
#define remote_log_warning() RemoteLogger::get().warning()
#define remote_log_error() RemoteLogger::get().error()
