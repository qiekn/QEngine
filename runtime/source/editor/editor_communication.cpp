#include "editor/editor_communication.h"
#include <iostream>
#include <chrono>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "editor/editor_event.h"

EditorCommunication::EditorCommunication()
    : m_running(false)
    , m_initialized(false)
    , m_context(1)
    , m_publisher(m_context, zmq::socket_type::pub)
    , m_subscriber(m_context, zmq::socket_type::sub) {
}

EditorCommunication::~EditorCommunication() {
    shutdown();
}

bool EditorCommunication::initialize() {
    if (m_initialized)
        return true;
    
    try {
        m_subscriber.connect("tcp://localhost:5555");
        m_publisher.connect("tcp://localhost:5556");
        
        m_subscriber.set(zmq::sockopt::subscribe, "");
        
        m_running = true;
        m_receive_thread = std::thread(&EditorCommunication::receive_messages, this);
        
        m_initialized = true;
        std::cout << "EditorCommunication initialized successfully" << std::endl;
        return true;
    }
    catch (const zmq::error_t& e) {
        std::cerr << "ZeroMQ error: " << e.what() << std::endl;
        return false;
    }
}

void EditorCommunication::shutdown() {
    if (!m_initialized)
        return;
    
    m_running = false;
    
    if (m_receive_thread.joinable()) {
        m_receive_thread.join();
    }
    
    m_initialized = false;
    std::cout << "EditorCommunication shut down" << std::endl;
}

bool EditorCommunication::send_message(const std::string& message) {
    if (!m_initialized) {
        std::cerr << "EditorCommunication not initialized" << std::endl;
        return false;
    }
    
    try {
        zmq::message_t zmq_message(message.size());
        memcpy(zmq_message.data(), message.data(), message.size());
        
        auto result = m_publisher.send(zmq_message, zmq::send_flags::none);
        return result.has_value();
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to send message: " << e.what() << std::endl;
        return false;
    }
}

void EditorCommunication::heartbeet() {
    rapidjson::Document msg;
    msg.SetObject();
    msg.AddMember("type", "heartbeet", msg.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    msg.Accept(writer);;

    if (!send_message(buffer.GetString())) {
        std::cerr << "Failed to send engine started notification" << std::endl;
    }
}

void EditorCommunication::receive_messages() {
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

void EditorCommunication::raise_events() {

    while(!m_message_queue.empty()) {
        const auto& msg = m_message_queue.front();

        rapidjson::Document doc;
        doc.Parse(msg.c_str());

        assert(!doc.HasParseError());
        assert(doc.HasMember("type"));
        assert(doc["type"].IsString());

        const std::string& type = doc["type"].GetString();

        if(type == "entity_property_changed") {
            EditorEventBus::get().publish<const rapidjson::Document&>(EditorEvent::EntityPropertyChanged, doc);
        }
        else if (type == "entity_variant_added") {
            EditorEventBus::get().publish<const rapidjson::Document&>(EditorEvent::EntityVariantAdded, doc);
        }
        else if (type == "enter_play_mode") {
            bool is_paused = doc["is_paused"].GetBool();
            EditorEventBus::get().publish<bool>(EditorEvent::EnterPlayMode, is_paused);
            std::cout << "Engine enter play mode >> is_paused: " << is_paused << std::endl;
        }
        else if (type == "exit_play_mode") {
            EditorEventBus::get().publish<bool>(EditorEvent::ExitPlayMode, false);
            std::cout << "Engine exit play mode" << std::endl;
        }

        m_message_queue.pop();
    }
}








