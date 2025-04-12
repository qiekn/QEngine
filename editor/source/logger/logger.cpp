#include "logger.h"
#include <iostream>

LogStream::LogStream(Logger& logger, LogLevel level) 
    : m_logger(logger), m_level(level) {
}

LogStream::~LogStream() {
    m_logger.log(m_level, m_stream.str());
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < m_min_log_level) {
        return;
    }
    
    std::string formattedMessage = level_to_string(level) + " " + message;

    if(level == LogLevel::ERROR) {
        std::cerr << formattedMessage << std::endl;
    }
    else {
        std::cout << formattedMessage << std::endl;
    }
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_logs.emplace_back(level, formattedMessage);
        
        constexpr size_t MAX_LOGS = 10000;
        if (m_logs.size() > MAX_LOGS) {
            m_logs.erase(m_logs.begin(), m_logs.begin() + (m_logs.size() - MAX_LOGS));
        }
        
        for (const auto& callback : m_callbacks) {
            callback(level, formattedMessage);
        }
    }
}
