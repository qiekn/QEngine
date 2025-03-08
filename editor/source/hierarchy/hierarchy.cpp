#include "hierarchy/hierarchy.h"

#include "rlImGui.h"
#include "imgui.h"
#include <iostream> // IWYU pragma: keep
#include <random>
#include <algorithm>

#include "imgui.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <map>

#include "engine/engine_event.h"

void notify_engine_about_change(const uint64_t entity_id, 
                                          const std::string& variant_type, 
                                          const std::string& key_type,
                                          const std::string& key_path, 
                                          const rapidjson::Value& new_value);
void Hierarchy::update() {
    if (ImGui::Begin("Entity List"))
    {
        render_create_entity();

        static bool saveRealtime = false;
        static float saveInterval = 1.0f;
        static float timeSinceLastSave = 0.0f;

        if (saveRealtime) {
            timeSinceLastSave += ImGui::GetIO().DeltaTime;
        }

        if (ImGui::Button("Save All")) {
            save_all_entities();
            timeSinceLastSave = 0.0f;
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Save Realtime", &saveRealtime)) {
            timeSinceLastSave = 0.0f;
        }

        if (saveRealtime) {
            ImGui::SameLine();

            float timeRemaining = saveInterval - timeSinceLastSave;

            if (timeSinceLastSave >= saveInterval) {
                save_all_entities();
                timeSinceLastSave = 0.0f;
            }
        }

        ImGui::Separator();

        for (auto& entity : m_entities) {
            if (entity.is_dead()) {
                continue;
            }

            render_entity(entity);
        }
    }
}

void Hierarchy::save_all_entities() {
    for (auto& entity : m_entities) {
        if (!entity.is_dead()) {
            entity.save_to_file();
        }
        else {
            entity.delete_entity_file();
        }
    }
}

void Hierarchy::render_create_entity() {
    constexpr size_t MAX_ENTITY_NAME_LENGTH = 128;
    static char newEntityName[MAX_ENTITY_NAME_LENGTH] = "";
    static bool showNewEntityPopup = false;
    
    if (ImGui::Button("+ Create New Entity", ImVec2(150, 20))) {
        memset(newEntityName, 0, sizeof(newEntityName));
        showNewEntityPopup = true;
    }
    
    ImGui::Spacing();
    
    if (showNewEntityPopup) {
        ImGui::OpenPopup("New Entity");
    }
    
    if (ImGui::BeginPopupModal("New Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a name for the new entity:");
        ImGui::InputTextWithHint("##EntityName", "Entity name", newEntityName, sizeof(newEntityName));
        ImGui::Spacing();
        
        bool isValidName = strlen(newEntityName) > 0;
        
        if (!isValidName) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            ImGui::Button("Create", ImVec2(100, 30));
            ImGui::PopStyleVar();
        } else if (ImGui::Button("Create", ImVec2(100, 30))) {
            // TODO: move random generation from here to its own module
            std::random_device rd;
            std::mt19937_64 gen(rd());
            std::uniform_int_distribution<uint64_t> dis;
            uint64_t uuid = dis(gen);
            
            std::string safeName = newEntityName;
            safeName.erase(std::remove_if(safeName.begin(), safeName.end(), 
                [](char c) { return c == '/' || c == '\\' || c == ':' || c == '*' || 
                                   c == '?' || c == '"' || c == '<' || c == '>' || c == '|'; }), 
                safeName.end());

            bool name_already_exists = false;

            for(auto& entity : m_entities) {
                if(entity.is_dead()) {
                    continue;
                }

                if(entity.get_name() == safeName) {
                    name_already_exists = true;
                }
            }

            if(!name_already_exists) {
                rapidjson::Document newDoc;
                newDoc.SetObject();
                rapidjson::Document::AllocatorType& allocator = newDoc.GetAllocator();
                
                newDoc.AddMember("entity_id", uuid, allocator);
                rapidjson::Value variantsArray(rapidjson::kArrayType);
                newDoc.AddMember("variants", variantsArray, allocator);
                
                EntityDocument entity(std::move(newDoc), safeName);
                m_entities.push_back(std::move(entity));
                
                ImGui::CloseCurrentPopup();
                showNewEntityPopup = false;
                memset(newEntityName, 0, sizeof(newEntityName));
            }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(100, 30))) {
            ImGui::CloseCurrentPopup();
            showNewEntityPopup = false;
        }
        
        ImGui::EndPopup();
    }
}

void Hierarchy::render_entity(EntityDocument& entity_document) {
    rapidjson::Document& document = entity_document.get_document();
    assert(!document.HasParseError());
    assert(entity_document.get_name() != "");

    uint64_t entity_id = 0;
    if (document.HasMember("entity_id")) {
        if (document["entity_id"].IsUint64()) {
            entity_id = document["entity_id"].GetUint64();
        }
    }

    const char* name = entity_document.get_name().c_str();

    ImGui::PushID(name);

    bool is_open = false;

    ImVec2 header_min = ImGui::GetCursorScreenPos();
    float header_height = ImGui::GetFrameHeight();

    is_open = ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen);

    ImVec2 header_max = ImVec2(
        ImGui::GetWindowContentRegionMax().x + ImGui::GetWindowPos().x,
        header_min.y + header_height
    );

    if (ImGui::IsMouseHoveringRect(header_min, header_max) && ImGui::IsMouseClicked(1)) {
        ImGui::OpenPopup("entity_context_menu");
    }

    if (ImGui::BeginPopup("entity_context_menu")) {
        if (ImGui::BeginMenu("Add Variant")) {
            for (int i = 0; i < m_variants.size(); i++) {
                const std::string& variant_name = m_variants[i].get_name();
                if (ImGui::MenuItem(variant_name.c_str())) {
                    add_variant_to_entity(entity_document, m_variants[i]);
                }
            }
            ImGui::EndMenu();
        }

        if(ImGui::MenuItem("Save as Variant")) {
            rapidjson::Document entity_as_var;
            entity_as_var.SetObject();

            entity_as_var.CopyFrom(entity_document.get_document(), entity_as_var.GetAllocator());

            if(!entity_as_var.HasMember("variants")) {
                rapidjson::Value variants_array(rapidjson::kArrayType);;
                entity_as_var.AddMember("variants", variants_array, entity_as_var.GetAllocator());
            }

            std::string variant_name = "[" + entity_document.get_name() + "]";
            std::filesystem::path variant_path = std::filesystem::path(VARIANT_FOLDER) / (variant_name + ".variant");

            VariantDocument variant(std::move(entity_as_var), variant_name);
            m_variants.push_back(std::move(variant));
        }

        if (ImGui::MenuItem("Delete Entity")) {
            entity_document.mark_as_dead();
        }
        ImGui::EndPopup();
    }

    if (is_open && document.HasMember("variants") && document["variants"].IsArray()) {
        rapidjson::Value& variants = document["variants"];
        for (rapidjson::SizeType i = 0; i < variants.Size(); ++i) {
            ImGui::PushID(i);

            ImVec2 variant_pos_min = ImGui::GetCursorScreenPos();

            render_variant(document, variants[i], i, entity_id);

            ImVec2 variant_pos_max = ImVec2(
                ImGui::GetWindowWidth() - 10,
                variant_pos_min.y + ImGui::GetFrameHeightWithSpacing()
            );

            char popup_name[32];
            snprintf(popup_name, sizeof(popup_name), "variant_context_menu_%d", i);

            if (ImGui::IsMouseHoveringRect(variant_pos_min, variant_pos_max) && ImGui::IsMouseClicked(1)) {
                ImGui::OpenPopup(popup_name);
            }

            if (ImGui::BeginPopup(popup_name)) {
                if (ImGui::MenuItem("Remove Variant")) {
                    variants.Erase(variants.Begin() + i);
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }
    }
    ImGui::PopID();
}

void Hierarchy::render_variant(rapidjson::Document& document, rapidjson::Value& variant,
                              int index, const uint64_t entity_id) {
    const char* type = variant["type"].GetString();

    ImGui::PushID(index);
    ImGui::Indent();

    if (ImGui::CollapsingHeader(type)) {
        if (variant.HasMember("value")) {
            render_object(document, variant["value"], entity_id, type);
        }
    }

    ImGui::Unindent();
    ImGui::PopID();
}
void Hierarchy::render_object(rapidjson::Document& document, rapidjson::Value& object, 
                             const uint64_t entity_id, const std::string& variant_type,
                             const std::string& parent_path) {
    static std::map<std::string, bool> editingField;

    ImGui::Indent(5);

    for (auto& member : object.GetObject()) {
        std::string key = member.name.GetString();
        rapidjson::Value& value = member.value;
        
        std::string current_path = parent_path.empty() ? key : parent_path + "." + key;
        std::string uniqueId = std::to_string(entity_id) + "_" + variant_type + "_" + current_path;
        
        ImGui::PushID(key.c_str());

        if (value.IsInt()) {
            int intValue = value.GetInt();
            
            ImGui::SetNextItemWidth(200.0f);
            bool edited = ImGui::InputInt(("Int: " + key).c_str(), &intValue);
            
            if (edited) {
                value.SetInt(intValue);
                editingField[uniqueId] = true;
            }
            
            if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                notify_engine_about_change(entity_id, variant_type, "int", current_path, value);
                editingField[uniqueId] = false;
            }
        }
        else if (value.IsFloat()) {
            float floatValue = value.GetFloat();
            
            ImGui::SetNextItemWidth(200.0f);
            bool edited = ImGui::InputFloat(("Float: " + key).c_str(), &floatValue, 0.1f, 1.0f, "%.3f");
            
            if (edited) {
                value.SetFloat(floatValue);
                editingField[uniqueId] = true;
            }
            
            if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                notify_engine_about_change(entity_id, variant_type, "float", current_path, value);
                editingField[uniqueId] = false;
            }
        }
        else if (value.IsBool()) {
            bool boolValue = value.GetBool();
            
            if (ImGui::Checkbox(("Bool: " + key).c_str(), &boolValue)) {
                value.SetBool(boolValue);
                
                notify_engine_about_change(entity_id, variant_type, "bool", current_path, value);
            }
        }
        else if (value.IsString()) {
            char buffer[256];
            strncpy(buffer, value.GetString(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';

            ImGui::SetNextItemWidth(200.0f);
            bool edited = ImGui::InputText(("String: " + key).c_str(), buffer, sizeof(buffer));

            if (edited) {
                value.SetString(buffer, document.GetAllocator());
                editingField[uniqueId] = true;
            }

            if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                notify_engine_about_change(entity_id, variant_type, "string",current_path, value);
                editingField[uniqueId] = false;
            }
        }
        else if (value.IsArray()) {
            if (ImGui::CollapsingHeader(("Array: " + key).c_str())) {
                ImGui::Indent();
                for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
                    ImGui::PushID(i);
                    
                    std::string array_path = current_path + "[" + std::to_string(i) + "]";
                    std::string arrayUniqueId = std::to_string(entity_id) + "_" + variant_type + "_" + array_path;
                    
                    rapidjson::Value& item = value[i];

                    if (item.IsObject()) {
                        if (ImGui::CollapsingHeader(("Item " + std::to_string(i)).c_str())) {
                            render_object(document, item, entity_id, variant_type, array_path);
                        }
                    }
                    else if (item.IsInt()) {
                        int intValue = item.GetInt();
                        
                        ImGui::SetNextItemWidth(200.0f);
                        bool edited = ImGui::InputInt(("Item " + std::to_string(i)).c_str(), &intValue);
                        
                        if (edited) {
                            item.SetInt(intValue);
                            editingField[arrayUniqueId] = true;
                        }
                        
                        if (editingField[arrayUniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                            notify_engine_about_change(entity_id, variant_type, "int", array_path, item);
                            editingField[arrayUniqueId] = false;
                        }
                    }
                    else if (item.IsFloat()) {
                        float floatValue = item.GetFloat();
                        
                        ImGui::SetNextItemWidth(200.0f);
                        bool edited = ImGui::InputFloat(("Item " + std::to_string(i)).c_str(), &floatValue);
                        
                        if (edited) {
                            item.SetFloat(floatValue);
                            editingField[arrayUniqueId] = true;
                        }
                        
                        if (editingField[arrayUniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                            notify_engine_about_change(entity_id, variant_type, "float", array_path, item);
                            editingField[arrayUniqueId] = false;
                        }
                    }
                    else if (item.IsBool()) {
                        bool boolValue = item.GetBool();
                        
                        if (ImGui::Checkbox(("Item " + std::to_string(i)).c_str(), &boolValue)) {
                            item.SetBool(boolValue);
                            notify_engine_about_change(entity_id, variant_type, "bool", array_path, item);
                        }
                    }
                    else if (item.IsString()) {
                        char buffer[256];
                        strncpy(buffer, item.GetString(), sizeof(buffer));
                        buffer[sizeof(buffer) - 1] = '\0';
                        
                        ImGui::SetNextItemWidth(200.0f);
                        bool edited = ImGui::InputText(("Item " + std::to_string(i)).c_str(), buffer, sizeof(buffer));
                        
                        if (edited) {
                            item.SetString(buffer, document.GetAllocator());
                            editingField[arrayUniqueId] = true;
                        }
                        
                        if (editingField[arrayUniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                            notify_engine_about_change(entity_id, variant_type, "string", array_path, item);
                            editingField[arrayUniqueId] = false;
                        }
                    }

                    ImGui::PopID();
                }
                ImGui::Unindent();
            }
        }
        else if (value.IsObject()) {
            if (ImGui::CollapsingHeader((key).c_str())) {
                render_object(document, value, entity_id, variant_type, current_path);
            }
        }
        ImGui::PopID();
    }
}

void Hierarchy::add_variant_to_entity(EntityDocument& entity_document, VariantDocument& variant_document) {
    rapidjson::Document& entity_doc = entity_document.get_document();
    const rapidjson::Document& variant_doc = variant_document.get_document();
    
    if (!entity_doc.HasMember("variants")) {
        rapidjson::Value variants_array(rapidjson::kArrayType);
        entity_doc.AddMember("variants", variants_array, entity_doc.GetAllocator());
    }
    
    rapidjson::Value& entity_variants = entity_doc["variants"];
    
    if (variant_doc.HasMember("variants") && variant_doc["variants"].IsArray()) { // entity variant case
        for (auto& variant : variant_doc["variants"].GetArray()) {
            rapidjson::Value copied_variant(variant, entity_doc.GetAllocator());
            entity_variants.PushBack(copied_variant, entity_doc.GetAllocator());
        }
    }
    else { 
        rapidjson::Value copied_variant(variant_doc, entity_doc.GetAllocator());
        entity_variants.PushBack(copied_variant, entity_doc.GetAllocator());
    }
}

void notify_engine_about_change(const uint64_t entity_id,
                                          const std::string& variant_type,
                                          const std::string& key_type,
                                          const std::string& key_path,
                                          const rapidjson::Value& new_value) {
    rapidjson::Document change_notification;
    change_notification.SetObject();

    std::string entity_id_str = std::to_string(entity_id);

    change_notification.AddMember("type",
        rapidjson::Value("entity_changed", change_notification.GetAllocator()),
        change_notification.GetAllocator());

    change_notification.AddMember("entity_id",
        rapidjson::Value(entity_id_str.c_str(), change_notification.GetAllocator()),
        change_notification.GetAllocator());

    change_notification.AddMember("variant_type",
        rapidjson::Value(variant_type.c_str(), change_notification.GetAllocator()),
        change_notification.GetAllocator());

    change_notification.AddMember("key_type",
        rapidjson::Value(key_type.c_str(), change_notification.GetAllocator()),
        change_notification.GetAllocator());

    change_notification.AddMember("key_path",
        rapidjson::Value(key_path.c_str(), change_notification.GetAllocator()),
        change_notification.GetAllocator());

    rapidjson::Value value_copy;
    value_copy.CopyFrom(new_value, change_notification.GetAllocator());
    change_notification.AddMember("value", value_copy, change_notification.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    change_notification.Accept(writer);

    EngineEventBus::get().publish<const std::string&>(EngineEvent::EntityModifiedEditor, buffer.GetString());
}
