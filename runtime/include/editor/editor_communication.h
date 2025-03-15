#pragma once
#ifdef EDITOR_MODE

#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <zmq.hpp>

class EditorCommunication {
public:
    EditorCommunication();
    ~EditorCommunication();

    bool initialize();
    void shutdown();
    bool send_message(const std::string& json);
    void raise_events();

private:
    void receive_messages();
    void start_connection_attempts();
    void send_started_message();
    
    bool m_running;
    bool m_initialized;

    zmq::context_t m_context;
    zmq::socket_t m_publisher;
    zmq::socket_t m_subscriber;
    std::thread m_receive_thread;
    std::mutex m_queue_mutex;
    std::queue<std::string> m_message_queue;
};

#endif
