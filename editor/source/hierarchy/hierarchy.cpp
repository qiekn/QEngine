#include "hierarchy/hierarchy.h"

#include <random>
#include <algorithm>
#include <map>
#include <fstream>

#include "imgui.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include "logger.h"
#include "engine/engine_event.h"

namespace {
    void notify_engine_entity_property_changed(uint64_t entity_id, 
                                              const std::string& variant_type, 
                                              const std::string& key_type,
                                              const std::string& key_path, 
                                              const std::string& new_value);

    void notify_engine_entity_variant_added(uint64_t entity_id, const std::string& variant_type);
    void notify_engine_entity_variant_removed(uint64_t entity_id, const std::string& variant_type);
    void notify_entity_removed(uint64_t entity_id);
}

Hierarchy::Hierarchy(std::vector<EntityDocument>& entities, std::vector<VariantDocument>& variants)
    : m_entities(entities), m_variants(variants)
{
    subscribe_events();
}

void Hierarchy::update() {
    render_create_entity();
    render_save_controls();
    ImGui::Separator();

    for (auto& entity : m_entities) {
        if (!entity.is_dead()) {
            render_entity(entity);
        }
    }
}

void Hierarchy::render_save_controls() {
    static bool save_real_time = false;
    static float save_interval = 1.0f;
    static float time_since_last_save = 0.0f;

    if (save_real_time) {
        time_since_last_save += ImGui::GetIO().DeltaTime;
    }

    if (ImGui::Button("Save All")) {
        save_all_entities();
        time_since_last_save = 0.0f;
    }

    ImGui::SameLine();
    if (ImGui::Checkbox("Save Realtime", &save_real_time)) {
        time_since_last_save = 0.0f;
    }

    if (save_real_time && time_since_last_save >= save_interval) {
        save_all_entities();
        time_since_last_save = 0.0f;
    }
}

void Hierarchy::save_all_entities() {
    for (auto& entity : m_entities) {
        if (!entity.is_dead()) {
            entity.save_to_file();
        } else {
            entity.delete_entity_file();
        }
    }
}

void Hierarchy::render_create_entity() {
    constexpr size_t MAX_ENTITY_NAME_LENGTH = 128;
    static char new_entity_name[MAX_ENTITY_NAME_LENGTH] = "";
    static bool show_new_entity_popup = false;

    if (ImGui::Button("+ Create New Entity", ImVec2(150, 20))) {
        memset(new_entity_name, 0, sizeof(new_entity_name));
        show_new_entity_popup = true;
    }

    ImGui::Spacing();

    if (show_new_entity_popup) {
        ImGui::OpenPopup("New Entity");
    }

    if (ImGui::BeginPopupModal("New Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a name for the new entity:");

        bool enter_pressed = ImGui::InputTextWithHint(
            "##EntityName",
            "Entity name",
            new_entity_name,
            sizeof(new_entity_name),
            ImGuiInputTextFlags_EnterReturnsTrue
        );

        ImGui::Spacing();

        bool is_valid_name = strlen(new_entity_name) > 0;

        if (enter_pressed && is_valid_name) {
            create_new_entity(new_entity_name);
            ImGui::CloseCurrentPopup();
            show_new_entity_popup = false;
            memset(new_entity_name, 0, sizeof(new_entity_name));
        } else {
            if (!is_valid_name) {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                ImGui::Button("Create", ImVec2(100, 30));
                ImGui::PopStyleVar();
            } else if (ImGui::Button("Create", ImVec2(100, 30))) {
                create_new_entity(new_entity_name);
                ImGui::CloseCurrentPopup();
                show_new_entity_popup = false;
                memset(new_entity_name, 0, sizeof(new_entity_name));
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(100, 30))) {
                ImGui::CloseCurrentPopup();
                show_new_entity_popup = false;
            }
        }

        ImGui::EndPopup();
    }
}

void Hierarchy::create_new_entity(const char* name) {
    if(name == nullptr) {
        log_error() << "Error: Cannot create entity with null name" << std::endl;
        return;
    }

    if(strlen(name) == 0) {
        log_error() << "Error: Cannot create entity with empty name" << std::endl;
        return;
    }


    std::string safe_name = name;
    safe_name.erase(std::remove_if(safe_name.begin(), safe_name.end(), 
        [](char c) { return c == '/' || c == '\\' || c == ':' || c == '*' || 
                           c == '?' || c == '"' || c == '<' || c == '>' || c == '|'; }), 
        safe_name.end());

    for (auto& entity : m_entities) {
        if (!entity.is_dead() && entity.get_name() == safe_name) {
            log_error() << "Error Entity with name " << safe_name << " already exists" << std::endl;
            return;
        }
    }

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    uint64_t uuid = dis(gen);
    
    rapidjson::Document new_doc;
    new_doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = new_doc.GetAllocator();
    
    new_doc.AddMember("entity_id", uuid, allocator);
    rapidjson::Value variantsArray(rapidjson::kArrayType);
    new_doc.AddMember("variants", variantsArray, allocator);

    EntityDocument entity(std::move(new_doc), safe_name);
    m_entities.push_back(std::move(entity));
}

void Hierarchy::render_entity(EntityDocument& entity_document) {
    rapidjson::Document& document = entity_document.get_document();
    if(!document.IsObject()) {
        return;
    }

    uint64_t entity_id = 0;
    if (document.HasMember("entity_id") && document["entity_id"].IsUint64()) {
        entity_id = document["entity_id"].GetUint64();
    }

    const char* name = entity_document.get_name().c_str();

    if(name == nullptr) {
        return;
    }

    ImGui::PushID(name);

    ImVec2 header_min = ImGui::GetCursorScreenPos();
    float header_height = ImGui::GetFrameHeight();
    bool is_open = ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen);
    ImVec2 header_max = ImVec2(
        ImGui::GetWindowContentRegionMax().x + ImGui::GetWindowPos().x,
        header_min.y + header_height
    );

    if (ImGui::IsMouseHoveringRect(header_min, header_max) && ImGui::IsMouseClicked(1)) {
        ImGui::OpenPopup("entity_context_menu");
    }

    handle_entity_context_menu(entity_document, entity_id);

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
                    std::string type = variants[i].GetObject()["type"].GetString();
                    notify_engine_entity_variant_removed(entity_id, type);
                    variants.Erase(variants.Begin() + i);
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }
    }
    ImGui::PopID();
}

void Hierarchy::handle_entity_context_menu(EntityDocument& entity_document, uint64_t entity_id) {
    if (ImGui::BeginPopup("entity_context_menu")) {
        if (ImGui::BeginMenu("Add Variant")) {
            for (int i = 0; i < m_variants.size(); i++) {
                if (m_variants[i].is_dead()) continue;
                
                const std::string& variant_name = m_variants[i].get_name();
                if (ImGui::MenuItem(variant_name.c_str())) {
                    add_variant_to_entity(entity_document, m_variants[i]);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Save as Variant")) {
            rapidjson::Document entity_as_var;
            entity_as_var.SetObject();
            entity_as_var.CopyFrom(entity_document.get_document(), entity_as_var.GetAllocator());

            if (!entity_as_var.HasMember("variants")) {
                rapidjson::Value variants_array(rapidjson::kArrayType);
                entity_as_var.AddMember("variants", variants_array, entity_as_var.GetAllocator());
            }

            std::string variant_name = "[" + entity_document.get_name() + "]";
            std::filesystem::path variant_path = std::filesystem::path(VARIANT_FOLDER) / (variant_name + ".variant");

            VariantDocument variant(std::move(entity_as_var), variant_name);
            m_variants.push_back(std::move(variant));
        }

        if (ImGui::MenuItem("Delete Entity")) {
            entity_document.mark_as_dead();
            notify_entity_removed(entity_id);
        }
        ImGui::EndPopup();
    }
}

void Hierarchy::render_variant(rapidjson::Document& document, rapidjson::Value& variant,
                              int index, uint64_t entity_id) {
    const char* type = variant["type"].GetString();

    ImGui::PushID(index);
    ImGui::Indent();

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.25f, 0.25f, 0.35f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.35f, 0.35f, 0.45f, 1.0f));

    bool is_open = ImGui::CollapsingHeader(type);

    ImGui::PopStyleColor(3);

    if (is_open && variant.HasMember("value")) {
        if (ImGui::BeginTable("variant_table", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

            render_object(document, variant["value"], entity_id, type);

            ImGui::EndTable();
        }
    }

    ImGui::Unindent();
    ImGui::PopID();
}

void Hierarchy::render_object(rapidjson::Document& document, rapidjson::Value& object,
                             uint64_t entity_id, const std::string& variant_type,
                             const std::string& parent_path) {
    if (!object.IsObject()) {
        return;
    }

    static std::map<std::string, bool> editingField;

    for (rapidjson::Value::MemberIterator it = object.MemberBegin(); it != object.MemberEnd(); ++it) {
        if (!it->name.IsString()) {
            continue;
        }

        std::string key = it->name.GetString();
        rapidjson::Value& value = it->value;

        std::string current_path = parent_path.empty() ? key : parent_path + "." + key;
        std::string safe_variant_type = variant_type.empty() ? "unknown_type" : variant_type;
        std::string uniqueId = std::to_string(entity_id) + "_" + safe_variant_type + "_" + current_path;

        ImGui::PushID(key.c_str());

        ImGui::TableNextRow();
        static int row_index = 0;
        row_index++;
        if (row_index % 2 == 0) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(50, 50, 60, 255));
        } else {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(40, 40, 48, 255));
        }

        ImGui::TableNextColumn();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.4f, 1.0f)); // Bright yellow-white
        ImGui::Text("%s", key.c_str());
        ImGui::PopStyleColor();

        ImGui::TableNextColumn();

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.24f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.25f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.3f, 0.3f, 0.36f, 1.0f));

        if (value.IsInt()) {
            int intValue = value.GetInt();

            ImGui::PushItemWidth(-1);
            bool edited = ImGui::InputInt("##int", &intValue, 1, 10);
            ImGui::PopItemWidth();

            if (edited) {
                value.SetInt(intValue);
                editingField[uniqueId] = true;
            }

            if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                std::string strValue = std::to_string(intValue);
                notify_engine_entity_property_changed(entity_id, variant_type, "int", current_path, strValue);
                editingField[uniqueId] = false;
            }
        }
        else if (value.IsFloat()) {
            float floatValue = value.GetFloat();

            ImGui::PushItemWidth(-1);
            bool edited = ImGui::DragFloat("##float", &floatValue, 0.1f, 0.0f, 0.0f, "%.3f");
            ImGui::PopItemWidth();

            if (edited) {
                value.SetFloat(floatValue);
                editingField[uniqueId] = true;
            }

            if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                std::string strValue = std::to_string(floatValue);
                notify_engine_entity_property_changed(entity_id, variant_type, "float", current_path, strValue);
                editingField[uniqueId] = false;
            }
        }
        else if (value.IsBool()) {
            bool boolValue = value.GetBool();

            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.22f, 0.22f, 0.27f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.27f, 0.27f, 0.33f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.0f, 1.0f, 1.0f, 1.0f)); 
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f); 
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.8f, 0.8f)); 

            float checkSize = ImGui::GetFrameHeight() * 1.2f;
            if (ImGui::Checkbox("##bool", &boolValue)) {
                value.SetBool(boolValue);
                std::string strValue = boolValue ? "true" : "false";
                notify_engine_entity_property_changed(entity_id, variant_type, "bool", current_path, strValue);
            }

            ImGui::SameLine();
            ImGui::TextColored(
                boolValue ? ImVec4(0.5f, 1.0f, 0.5f, 1.0f) : ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                boolValue ? "true" : "false"
            );

            ImGui::PopStyleColor(5);
            ImGui::PopStyleVar();
        }
        else if (value.IsString()) {
            char buffer[256];
            strncpy(buffer, value.GetString(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';

            ImGui::PushItemWidth(-1);
            bool edited = ImGui::InputText("##string", buffer, sizeof(buffer));
            ImGui::PopItemWidth();

            if (edited) {
                value.SetString(buffer, document.GetAllocator());
                editingField[uniqueId] = true;
            }

            if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
                std::string strValue = buffer;
                notify_engine_entity_property_changed(entity_id, variant_type, "string", current_path, strValue);
                editingField[uniqueId] = false;
            }
        }
        else if (value.IsObject()) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "[Object]");
        }
        else if (value.IsArray()) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "[Array: %d items]", value.Size());
        }
        else {
            ImGui::TextColored(ImVec4(0.7f, 0.5f, 0.5f, 1.0f), "[Unsupported type]");
        }

        ImGui::PopStyleColor(3); 
        ImGui::PopID();
    }
}

void Hierarchy::render_int_field(rapidjson::Document& document, rapidjson::Value& value,
                                uint64_t entity_id, const std::string& variant_type,
                                const std::string& key, const std::string& current_path,
                                const std::string& uniqueId, std::map<std::string, bool>& editingField) {
    int intValue = value.GetInt();

    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 1.0f, 1.0f), "%s:", key.c_str());
    ImGui::SameLine();

    ImGui::PushItemWidth(120.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.35f, 0.35f, 0.45f, 1.0f));

    char buf[16];
    snprintf(buf, sizeof(buf), "%d", intValue);

    bool edited = ImGui::InputInt("##int", &intValue, 1, 10);

    ImGui::PopStyleColor(3);
    ImGui::PopItemWidth();

    if (edited) {
        value.SetInt(intValue);
        editingField[uniqueId] = true;
    }

    if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
        std::string strValue = std::to_string(intValue);
        notify_engine_entity_property_changed(entity_id, variant_type, "int", current_path, strValue);
        editingField[uniqueId] = false;
    }
}

void Hierarchy::render_float_field(rapidjson::Document& document, rapidjson::Value& value,
                                  uint64_t entity_id, const std::string& variant_type,
                                  const std::string& key, const std::string& current_path,
                                  const std::string& uniqueId, std::map<std::string, bool>& editingField) {
    float floatValue = value.GetFloat();

    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(ImVec4(0.7f, 1.0f, 1.0f, 1.0f), "%s:", key.c_str());
    ImGui::SameLine();

    ImGui::PushItemWidth(120.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 0.3f, 1.0f));

    bool edited = ImGui::DragFloat("##float", &floatValue, 0.1f, 0.0f, 0.0f, "%.3f");

    ImGui::PopStyleColor(3);
    ImGui::PopItemWidth();

    if (edited) {
        value.SetFloat(floatValue);
        editingField[uniqueId] = true;
    }

    if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
        std::string strValue = std::to_string(floatValue);
        notify_engine_entity_property_changed(entity_id, variant_type, "float", current_path, strValue);
        editingField[uniqueId] = false;
    }
}

void Hierarchy::render_bool_field(rapidjson::Value& value, uint64_t entity_id,
                                 const std::string& variant_type, const std::string& key,
                                 const std::string& current_path) {
    bool boolValue = value.GetBool();

    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.7f, 1.0f), "%s:", key.c_str());
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.5f, 1.0f, 0.5f, 1.0f));

    if (ImGui::Checkbox("##bool", &boolValue)) {
        value.SetBool(boolValue);

        std::string strValue = boolValue ? "true" : "false";
        notify_engine_entity_property_changed(entity_id, variant_type, "bool", current_path, strValue);
    }

    ImGui::PopStyleColor(3);
}

void Hierarchy::render_string_field(rapidjson::Document& document, rapidjson::Value& value,
                                   uint64_t entity_id, const std::string& variant_type,
                                   const std::string& key, const std::string& current_path,
                                   const std::string& uniqueId, std::map<std::string, bool>& editingField) {
    char buffer[256];
    strncpy(buffer, value.GetString(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    ImGui::AlignTextToFramePadding();
    ImGui::TextColored(ImVec4(1.0f, 0.7f, 1.0f, 1.0f), "%s:", key.c_str());
    ImGui::SameLine();

    ImGui::PushItemWidth(200.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 0.3f, 1.0f));

    bool edited = ImGui::InputText("##string", buffer, sizeof(buffer));

    ImGui::PopStyleColor(3);
    ImGui::PopItemWidth();

    if (edited) {
        value.SetString(buffer, document.GetAllocator());
        editingField[uniqueId] = true;
    }

    if (editingField[uniqueId] && ImGui::IsItemDeactivatedAfterEdit()) {
        std::string strValue = buffer;
        notify_engine_entity_property_changed(entity_id, variant_type, "string", current_path, strValue);
        editingField[uniqueId] = false;
    }
}

void Hierarchy::render_array_field(rapidjson::Document& document, rapidjson::Value& value,
                                  uint64_t entity_id, const std::string& variant_type,
                                  const std::string& key, const std::string& current_path,
                                  std::map<std::string, bool>& editingField) {
    if (ImGui::CollapsingHeader((key + " : Arr").c_str())) {
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
                render_int_field(document, item, entity_id, variant_type, "Item " + std::to_string(i), 
                                array_path, arrayUniqueId, editingField);
            }
            else if (item.IsFloat()) {
                render_float_field(document, item, entity_id, variant_type, "Item " + std::to_string(i), 
                                  array_path, arrayUniqueId, editingField);
            }
            else if (item.IsBool()) {
                render_bool_field(item, entity_id, variant_type, "Item " + std::to_string(i), array_path);
            }
            else if (item.IsString()) {
                render_string_field(document, item, entity_id, variant_type, "Item " + std::to_string(i), 
                                   array_path, arrayUniqueId, editingField);
            }

            ImGui::PopID();
        }
        ImGui::Unindent();
    }
}

void Hierarchy::add_variant_to_entity(EntityDocument& entity_document, VariantDocument& variant_document) {
    rapidjson::Document& entity_doc = entity_document.get_document();
    const rapidjson::Document& variant_doc = variant_document.get_document();
    
    uint64_t entity_id = entity_doc["entity_id"].GetUint64();

    if (!entity_doc.HasMember("variants")) {
        rapidjson::Value variants_array(rapidjson::kArrayType);
        entity_doc.AddMember("variants", variants_array, entity_doc.GetAllocator());
    }

    rapidjson::Value& entity_variants = entity_doc["variants"];

    if (variant_doc.HasMember("variants") && variant_doc["variants"].IsArray()) {
        for (auto& variant : variant_doc["variants"].GetArray()) {
            rapidjson::Value copied_variant(variant, entity_doc.GetAllocator());
            entity_variants.PushBack(copied_variant, entity_doc.GetAllocator());
            const auto& type = variant["type"].GetString();
            notify_engine_entity_variant_added(entity_id, type);
        }
    }
    else {
        const std::string& type = variant_doc["type"].GetString();

        for(const auto& variant : entity_document.get_document()["variants"].GetArray()) {
            if(variant["type"].GetString() == type) {
                std::cout << type << " is already added to entity: " << entity_document.get_name() << std::endl;
                return;
            }
        }

        rapidjson::Value copied_variant(variant_doc, entity_doc.GetAllocator());
        entity_variants.PushBack(copied_variant, entity_doc.GetAllocator());
        notify_engine_entity_variant_added(entity_id, type);
        add_required_variants_to_entity(entity_document, type);
    }
}

void Hierarchy::add_required_variants_to_entity(EntityDocument& entity_document, const std::string& variant_type) {
    std::string requires_path = std::string(VARIANT_FOLDER) + "/requires/" + variant_type + ".requires";

    if (!std::filesystem::exists(requires_path)) {
        return;
    }

    log_info() << "Found requirements file for " << variant_type << std::endl;

    try {
        std::ifstream requires_file(requires_path);
        if (!requires_file.is_open()) {
            log_error() << "Failed to open requires file: " << requires_path << std::endl;
            return;
        }

        std::string json_str((std::istreambuf_iterator<char>(requires_file)), std::istreambuf_iterator<char>());
        requires_file.close();

        rapidjson::Document requires_doc;
        requires_doc.Parse(json_str.c_str());

        if (requires_doc.HasParseError()) {
            log_error() << "Error parsing requires file: " << requires_path << std::endl;
            return;
        }

        if (!requires_doc.HasMember("requires") || !requires_doc["requires"].IsArray()) {
            log_error() << "Invalid requires file format: " << requires_path << std::endl;
            return;
        }

        const auto& required_variants = requires_doc["requires"].GetArray();
        for (rapidjson::SizeType i = 0; i < required_variants.Size(); i++) {
            if (!required_variants[i].IsString()) {
                continue;
            }

            std::string required_type = required_variants[i].GetString();

            bool variant_already_exists = false;
            for (const auto& variant : entity_document.get_document()["variants"].GetArray()) {
                if (std::string(variant["type"].GetString()) == required_type) {
                    variant_already_exists = true;
                    break;
                }
            }

            if (variant_already_exists) {
                log_info() << "Required variant " << required_type << " already exists on entity" << std::endl;
                continue;
            }

            bool variant_found = false;
            for (auto& variant : m_variants) {
                if (variant.is_dead()) continue;

                if (variant.get_name() == required_type) {
                    log_info() << "Adding required variant " << required_type << " to entity" << std::endl;
                    add_variant_to_entity(entity_document, variant);
                    variant_found = true;
                    break;
                }
            }

            if (!variant_found) {
                log_warning() << "Required variant " << required_type << " was not found in variant list" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        log_error() << "Exception processing requires file: " << e.what() << std::endl;
    }
}

void Hierarchy::subscribe_events() {}

namespace {
    void notify_engine_entity_property_changed(uint64_t entity_id,
                                              const std::string& variant_type,
                                              const std::string& key_type,
                                              const std::string& key_path,
                                              const std::string& new_value) {
        rapidjson::Document change_notification;
        change_notification.SetObject();
        
        rapidjson::Value type_value(variant_type.c_str(), change_notification.GetAllocator());
        rapidjson::Value key_type_value(key_type.c_str(), change_notification.GetAllocator());
        rapidjson::Value key_path_value(key_path.c_str(), change_notification.GetAllocator());
        rapidjson::Value value_str(new_value.c_str(), change_notification.GetAllocator());
        
        change_notification.AddMember("type", 
                                     rapidjson::Value("entity_property_changed", change_notification.GetAllocator()),
                                     change_notification.GetAllocator());
        change_notification.AddMember("entity_id", entity_id, change_notification.GetAllocator());
        change_notification.AddMember("variant_type", type_value, change_notification.GetAllocator());
        change_notification.AddMember("key_type", key_type_value, change_notification.GetAllocator());
        change_notification.AddMember("key_path", key_path_value, change_notification.GetAllocator());
        change_notification.AddMember("value", value_str, change_notification.GetAllocator());
        
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        change_notification.Accept(writer);
        
        EngineEventBus::get().publish<const std::string&>(EngineEvent::EntityModifiedEditor, buffer.GetString());
    }

    void notify_engine_entity_variant_added(uint64_t entity_id, const std::string& type) {
        rapidjson::Document msg;
        msg.SetObject();

        rapidjson::Value variant_type(type.c_str(), msg.GetAllocator());

        msg.AddMember("type", "entity_variant_added", msg.GetAllocator());
        msg.AddMember("entity_id", entity_id, msg.GetAllocator());
        msg.AddMember("variant_type", variant_type, msg.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        msg.Accept(writer);

        EngineEventBus::get().publish<const std::string&>(EngineEvent::EntityModifiedEditor, buffer.GetString());
    }

    void notify_engine_entity_variant_removed(uint64_t entity_id, const std::string& type) {
        rapidjson::Document msg;
        msg.SetObject();

        rapidjson::Value variant_type(type.c_str(), msg.GetAllocator());

        msg.AddMember("type", "entity_variant_removed", msg.GetAllocator());
        msg.AddMember("entity_id", entity_id, msg.GetAllocator());
        msg.AddMember("variant_type", variant_type, msg.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        msg.Accept(writer);

        EngineEventBus::get().publish<const std::string&>(EngineEvent::EntityModifiedEditor, buffer.GetString());
    }

    void notify_entity_removed(uint64_t entity_id) {
        rapidjson::Document msg;
        msg.SetObject();

        msg.AddMember("type", "entity_removed", msg.GetAllocator());
        msg.AddMember("entity_id", entity_id, msg.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        msg.Accept(writer);

        EngineEventBus::get().publish<const std::string&>(EngineEvent::EntityModifiedEditor, buffer.GetString());
    }
}
