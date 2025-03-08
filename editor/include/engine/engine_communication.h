#pragma once

#include <functional>
#include <atomic>
#include <string>
#include <thread>
#include <queue>
#include <mutex>

#include "zmq/zmq.hpp"

class EngineCommunication {
public:
    EngineCommunication();
    ~EngineCommunication();

    bool initialize();
    void shutdown();
    bool send_message(const std::string& json);
    bool is_engine_connected() const;
    void process_recieved_messages();

private:
    void recieve_messages();

    std::atomic<bool> m_running;
    bool m_initialized;
    
    zmq::context_t m_context;
    zmq::socket_t m_publisher;  
    zmq::socket_t m_subscriber;
    
    std::thread m_receive_thread;
    std::queue<std::string> m_message_queue;
    std::mutex m_queue_mutex;
};
