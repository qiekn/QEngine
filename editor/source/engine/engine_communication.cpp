#include "engine/engine_communication.h"

#include <iostream>
#include <chrono>

#include "engine/engine_event.h"
#include "rapidjson/document.h"

EngineCommunication::EngineCommunication()
    : m_running(false)
    , m_initialized(false)
    , m_context(1)
    , m_publisher(m_context, zmq::socket_type::pub)
    , m_subscriber(m_context, zmq::socket_type::sub) {

        EngineEventBus::get().subscribe<const std::string&>(EngineEvent::EntityModifiedEditor, [this](const std::string& msg) {
                send_message(msg);
        });
}

EngineCommunication::~EngineCommunication() {
    shutdown();
}

bool EngineCommunication::initialize() {
    if (m_initialized)
        return true;
    
    try {
        m_publisher.bind("tcp://*:5555");
        m_subscriber.bind("tcp://*:5556");
        m_subscriber.set(zmq::sockopt::subscribe, "");
        
        m_running = true;
        m_receive_thread = std::thread(&EngineCommunication::recieve_messages, this);
        
        m_initialized = true;
        return true;
    }
    catch (const zmq::error_t& e) {
        std::cerr << "ZeroMQ error: " << e.what() << std::endl;
        return false;
    }
}

void EngineCommunication::shutdown() {
    if (!m_initialized)
        return;
    
    m_running = false;
    
    if (m_receive_thread.joinable()) {
        m_receive_thread.join();
    }
    
    m_initialized = false;
}

bool EngineCommunication::send_message(const std::string& json) {
    if (!m_initialized) {
        std::cerr << "EngineCommunication not initialized" << std::endl;
        return false;
    }
    
    try {
        zmq::message_t message(json.size());
        memcpy(message.data(), json.data(), json.size());
        
        auto result = m_publisher.send(message, zmq::send_flags::none);
        return result.has_value();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to send message: " << e.what() << std::endl;
        return false;
    }
}

bool EngineCommunication::is_engine_connected() const {
    return m_initialized;
}

void EngineCommunication::recieve_messages() {
    while (m_running) {
        try {
            zmq::pollitem_t items[] = {
                { static_cast<void*>(m_subscriber), 0, ZMQ_POLLIN, 0 }
            };
            
            zmq::poll(items, 1, std::chrono::milliseconds(100));
            
            if (items[0].revents & ZMQ_POLLIN) {
                zmq::message_t message;
                auto result = m_subscriber.recv(message, zmq::recv_flags::none);
                
                if (result.has_value()) {
                    std::string message_str(static_cast<char*>(message.data()), message.size());
                    
                    {
                        std::lock_guard<std::mutex> lock(m_queue_mutex);
                        m_message_queue.push(message_str);
                    }
                }
            }
        }
        catch (const zmq::error_t& e) {
            std::cerr << "Error receiving message: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void EngineCommunication::raise_events() {
    rapidjson::Document doc;

    while(!m_message_queue.empty()) {
        const std::string& msg = m_message_queue.front();
        doc.Parse(msg.c_str());

        assert(doc.HasMember("type"));

        const std::string& type = doc["type"].GetString();

        if(type == "heartbeet") {
            EngineEventBus::get().publish<bool>(EngineEvent::EngineHeartbeet, true);
            EngineEventBus::get().publish<bool>(EngineEvent::EngineStarted, true);
        }

        m_message_queue.pop();
    }

}












