#include "hierarchy/hierarchy.h"

#include <iostream> // IWYU pragma: keep
#include <random>
#include <algorithm>
#include <thread>

#include "imgui.h"

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
    ignore_file_events = true;

    for (auto& entity : m_entities) {
        if (!entity.is_dead()) {
            entity.save_to_file();
        }
        else {
            entity.delete_entity_file();
        }
    }

    std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            ignore_file_events = false;
    }).detach();
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

        render_variant(document, variants[i], i);

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

void Hierarchy::render_variant(rapidjson::Document& document, rapidjson::Value& variant, int index) {
    const char* type = variant["type"].GetString();

    ImGui::PushID(index);
    ImGui::Indent();

    if (ImGui::CollapsingHeader(type)) {
        if (variant.HasMember("value")) {
            render_object(document, variant["value"]);
        }
    }

    ImGui::Unindent();
    ImGui::PopID();
}

void Hierarchy::render_object(rapidjson::Document& document, rapidjson::Value& object) {
    for (auto& member : object.GetObject()) {
        std::string key = member.name.GetString();
        rapidjson::Value& value = member.value;

        ImGui::PushID(key.c_str());

    if (value.IsInt()) {
        int intValue = value.GetInt();
        if (ImGui::InputInt(("Int: " + key).c_str(), &intValue)) {
            value.SetInt(intValue);
        }
    }
    else if (value.IsFloat()) {
        float floatValue = value.GetFloat();
        if (ImGui::InputFloat(("Float: " + key).c_str(), &floatValue, 0.1f, 1.0f, "%.3f")) {
            value.SetFloat(floatValue);
        }
    }
    else if (value.IsBool()) {
        bool boolValue = value.GetBool();
        if (ImGui::Checkbox(("Bool: " + key).c_str(), &boolValue)) {
            value.SetBool(boolValue);
        }
    }
    else if (value.IsString()) {
        std::string strValue = value.GetString();
    char buffer[256];
        strncpy(buffer, strValue.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText(("String: " + key).c_str(), buffer, sizeof(buffer))) {
            value.SetString(buffer, document.GetAllocator());
        }
    }
    else if (value.IsArray()) {
        if (ImGui::CollapsingHeader(("Array: " + key).c_str())) {
            ImGui::Indent();
            for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
                ImGui::PushID(i);

                rapidjson::Value& item = value[i];

                if (item.IsObject()) {
                    if (ImGui::CollapsingHeader(("Item " + std::to_string(i)).c_str())) {
                    }
                }
                else if (item.IsInt()) {
                    int intValue = item.GetInt();
                    if (ImGui::InputInt(("Item " + std::to_string(i)).c_str(), &intValue)) {
                        item.SetInt(intValue);
                    }
                }
                else if (item.IsFloat()) {
                    float floatValue = item.GetFloat();
                    if (ImGui::InputFloat(("Item " + std::to_string(i)).c_str(), &floatValue)) {
                        item.SetFloat(floatValue);
                    }
                }
                else if (item.IsBool()) {
                    bool boolValue = item.GetBool();
                    if (ImGui::Checkbox(("Item " + std::to_string(i)).c_str(), &boolValue)) {
                        item.SetBool(boolValue);
                    }
                }
                else if (item.IsString()) {
                    std::string strValue = item.GetString();
                    char buffer[256];
                    strncpy(buffer, strValue.c_str(), sizeof(buffer));
                    buffer[sizeof(buffer) - 1] = '\0';

                    if (ImGui::InputText(("Item " + std::to_string(i)).c_str(), buffer, sizeof(buffer))) {
                            item.SetString(buffer, document.GetAllocator());
                        }
                    }

                    ImGui::PopID();
                }
                ImGui::Unindent();
            }
        }
    else if (value.IsObject()) {
        if (ImGui::CollapsingHeader((key).c_str())) {
            ImGui::Indent();
            render_object(document, value);
            ImGui::Unindent();
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
    else { // plain variant case
        rapidjson::Value copied_variant(variant_doc, entity_doc.GetAllocator());
        entity_variants.PushBack(copied_variant, entity_doc.GetAllocator());
    }
}



