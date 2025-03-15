#include "engine/engine_communication.h"

#include <iostream>
#include <chrono>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "engine/engine_event.h"

EngineCommunication::EngineCommunication()
    : m_running(false)
    , m_initialized(false)
    , m_context(1)
    , m_publisher(m_context, zmq::socket_type::pub)
    , m_subscriber(m_context, zmq::socket_type::sub) 
{
    register_event_handlers();
}

EngineCommunication::~EngineCommunication() {
    shutdown();
}

void EngineCommunication::register_event_handlers() {
    EngineEventBus::get().subscribe<const std::string&>(EngineEvent::EngineSendScene,
            [this](const auto& msg) {
                send_message(msg);
    });

    EngineEventBus::get().subscribe<const std::string&>(
        EngineEvent::EntityModifiedEditor, 
        [this](const std::string& msg) {
            send_message(msg);
        }
    );

    EngineEventBus::get().subscribe<bool>(
        EngineEvent::EnterPlayMode, 
        [this](bool paused) {
            send_simple_message("enter_play_mode", "is_paused", paused);
        }
    );

    EngineEventBus::get().subscribe<bool>(
        EngineEvent::ExitPlayMode, 
        [this](bool) {
            send_simple_message("exit_play_mode");
        }
    );

    EngineEventBus::get().subscribe<bool>(
        EngineEvent::PausePlayMode, 
        [this](bool) {
            send_simple_message("pause_play_mode");
        }
    );

    EngineEventBus::get().subscribe<bool>(
        EngineEvent::UnPausePlayMode, 
        [this](bool) {
            send_simple_message("unpause_play_mode");
        }
    );
}

bool EngineCommunication::initialize() {
    if (m_initialized)
        return true;
    
    try {
        m_publisher.bind("tcp://*:5555");
        m_subscriber.bind("tcp://*:5556");
        m_subscriber.set(zmq::sockopt::subscribe, "");
        
        m_running = true;
        m_receive_thread = std::thread(&EngineCommunication::receive_messages, this);
        
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

bool EngineCommunication::send_simple_message(const std::string& type,
                                             const std::string& key,
                                             bool value) {
    rapidjson::Document msg;
    msg.SetObject();

    rapidjson::Value typeValue(type.c_str(), msg.GetAllocator());
    msg.AddMember("type", typeValue, msg.GetAllocator());

    if (!key.empty()) {
        rapidjson::Value keyValue(key.c_str(), msg.GetAllocator());
        rapidjson::Value boolValue(value);
        msg.AddMember(keyValue, boolValue, msg.GetAllocator());
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    msg.Accept(writer);

    return send_message(buffer.GetString());
}

bool EngineCommunication::is_engine_connected() const {
    return m_initialized;
}

void EngineCommunication::receive_messages() {
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
                    
                    std::lock_guard<std::mutex> lock(m_queue_mutex);
                    m_message_queue.push(message_str);
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
    std::queue<std::string> messages;
    
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        messages.swap(m_message_queue);
    }
    
    while (!messages.empty()) {
        const std::string& msg = messages.front();
        
        rapidjson::Document doc;
        doc.Parse(msg.c_str());
        
        if (doc.HasParseError() || !doc.HasMember("type")) {
            std::cerr << "Invalid message format received" << std::endl;
            messages.pop();
            continue;
        }
        
        const std::string& type = doc["type"].GetString();
        
        if (type == "scene") {
            EngineEventBus::get().publish<rapidjson::Document>(EngineEvent::SyncEditor, doc);
        }
        else if (type == "engine_started") {
            send_simple_message("engine_start_confirmed");
            EngineEventBus::get().publish<bool>(EngineEvent::EngineStarted, true);
        }
        else {
            std::cout << "Editor: Unknown message received from engine: " << type << std::endl;
        }
        
        messages.pop();
    }
}
