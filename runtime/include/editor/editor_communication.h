#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <map>
#include <mutex>
#include <queue>

#include "zmq/zmq.hpp"

class EditorCommunication {
public:
    EditorCommunication();
    ~EditorCommunication();

    bool initialize();
    void shutdown();
    bool send_message(const std::string& message);
    void notify_engine_started();
    void process_messages();

private:
    void receive_messages();
    
    std::atomic<bool> m_running;
    bool m_initialized;
    
    zmq::context_t m_context;
    zmq::socket_t m_publisher;  
    zmq::socket_t m_subscriber;
    
    std::thread m_receive_thread;
    std::queue<std::string> m_message_queue;
    std::mutex m_queue_mutex;
};
