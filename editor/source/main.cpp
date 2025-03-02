#include <iostream>
#include <fstream>
#include <filesystem>

#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

namespace fs = std::filesystem;

void SaveJsonToFile(const fs::path& filePath, const rapidjson::Document& document) {
    std::ofstream outFile(filePath);
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing: " << filePath << std::endl;
        return;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    outFile << buffer.GetString();
    outFile.close();
}

bool LoadJsonFromFile(const fs::path& filePath, rapidjson::Document& document) {
    std::ifstream inFile(filePath);
    if (!inFile) {
        std::cerr << "Error: Unable to open file: " << filePath << std::endl;
        return false;
    }

    std::string jsonString((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    document.Parse(jsonString.c_str());
    if (document.HasParseError()) {
        std::cerr << "Error parsing JSON in file: " << filePath << std::endl;
        return false;
    }

    return true;
}

void RenderJsonObject(rapidjson::Value& object, const fs::path& filePath, rapidjson::Document& document) {
    for (auto& member : object.GetObject()) {
        std::string key = member.name.GetString();
        rapidjson::Value& value = member.value;

        ImGui::PushID(key.c_str());

        if (value.IsInt()) {
            int intValue = value.GetInt();
            if (ImGui::InputInt(("Int: " + key).c_str(), &intValue)) {
                value.SetInt(intValue);
                SaveJsonToFile(filePath, document);
            }
        }
        else if (value.IsFloat()) {
            float floatValue = value.GetFloat();
            if (ImGui::InputFloat(("Float: " + key).c_str(), &floatValue, 0.1f, 1.0f, "%.3f")) {
                value.SetFloat(floatValue);
                SaveJsonToFile(filePath, document);
            }
        }
        else if (value.IsBool()) {
            bool boolValue = value.GetBool();
            if (ImGui::Checkbox(("Bool: " + key).c_str(), &boolValue)) {
                value.SetBool(boolValue);
                SaveJsonToFile(filePath, document);
            }
        }
        else if (value.IsString()) {
            std::string strValue = value.GetString();
            char buffer[256];  
            strncpy(buffer, strValue.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0';

            if (ImGui::InputText(("String: " + key).c_str(), buffer, sizeof(buffer))) {
                value.SetString(buffer, document.GetAllocator());
                SaveJsonToFile(filePath, document);
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
                            RenderJsonObject(item, filePath, document);
                        }
                    } 
                    else if (item.IsInt()) {
                        int intValue = item.GetInt();
                        if (ImGui::InputInt(("Item " + std::to_string(i)).c_str(), &intValue)) {
                            item.SetInt(intValue);
                            SaveJsonToFile(filePath, document);
                        }
                    } 
                    else if (item.IsFloat()) {
                        float floatValue = item.GetFloat();
                        if (ImGui::InputFloat(("Item " + std::to_string(i)).c_str(), &floatValue)) {
                            item.SetFloat(floatValue);
                            SaveJsonToFile(filePath, document);
                        }
                    } 
                    else if (item.IsBool()) {
                        bool boolValue = item.GetBool();
                        if (ImGui::Checkbox(("Item " + std::to_string(i)).c_str(), &boolValue)) {
                            item.SetBool(boolValue);
                            SaveJsonToFile(filePath, document);
                        }
                    } 
                    else if (item.IsString()) {
                        std::string strValue = item.GetString();
                        char buffer[256];  
                        strncpy(buffer, strValue.c_str(), sizeof(buffer));
                        buffer[sizeof(buffer) - 1] = '\0';

                        if (ImGui::InputText(("Item " + std::to_string(i)).c_str(), buffer, sizeof(buffer))) {
                            item.SetString(buffer, document.GetAllocator());
                            SaveJsonToFile(filePath, document);
                        }
                    }

                    ImGui::PopID();
                }
                ImGui::Unindent();
            }
        }
        else if (value.IsObject()) {
            if (ImGui::CollapsingHeader(("Object: " + key).c_str())) {
                ImGui::Indent();
                RenderJsonObject(value, filePath, document);
                ImGui::Unindent();
            }
        }

        ImGui::PopID();
    }
}

void RenderVariant(rapidjson::Value& variant, const fs::path& filePath, rapidjson::Document& document, int index) {
    const char* type = variant["type"].GetString();

    ImGui::PushID(index);
    ImGui::Indent();

    if (ImGui::CollapsingHeader(type)) {
        if (variant.HasMember("value")) {
            RenderJsonObject(variant["value"], filePath, document);
        }
    }

    ImGui::Unindent();
    ImGui::PopID();
}

void ShowJsonEditor(const fs::path& folderPath) {
    static char newEntityName[128] = "";
    static bool showNewEntityPopup = false;

    if (ImGui::Button("Create New Entity")) {
        showNewEntityPopup = true;
    }

    if (showNewEntityPopup) {
        ImGui::OpenPopup("New Entity");
    }

    if (ImGui::BeginPopupModal("New Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Entity Name", newEntityName, sizeof(newEntityName));
        if (ImGui::Button("Create")) {
            fs::path filePath = folderPath / (std::string(newEntityName) + ".entity");

            rapidjson::Document newDoc;
            newDoc.SetObject();
            rapidjson::Document::AllocatorType& allocator = newDoc.GetAllocator();

            newDoc.AddMember("id", 123456789, allocator);

            rapidjson::Value variants(rapidjson::kArrayType);
            newDoc.AddMember("variants", variants, allocator);

            SaveJsonToFile(filePath, newDoc);
            ImGui::CloseCurrentPopup();
            showNewEntityPopup = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            showNewEntityPopup = false;
        }
        ImGui::EndPopup();
    }

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        std::cerr << "Error: Folder does not exist or is not a directory: " << folderPath << std::endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".entity") {
            fs::path filePath = entry.path();
            std::string fileName = filePath.stem().string();

            rapidjson::Document document;
            if (!LoadJsonFromFile(filePath, document)) {
                continue;
            }

            ImGui::PushID(fileName.c_str());
            if (ImGui::CollapsingHeader(fileName.c_str())) {
                if (document.HasMember("variants")) {
                    rapidjson::Value& variants = document["variants"];
                    for (rapidjson::SizeType i = 0; i < variants.Size(); ++i) {
                        RenderVariant(variants[i], filePath, document, i);
                    }
                }
            }
            ImGui::PopID();
        }
    }
}

int main(int argc, char* argv[])
{
	int screenWidth = 1280;
	int screenHeight = 800;

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Zeytin");
	SetTargetFPS(144);
	rlImGuiSetup(true);

	while (!WindowShouldClose())    
	{
		BeginDrawing();
		ClearBackground(DARKGRAY);

		rlImGuiBegin();

		bool open = true;

		open = true;
		

        if (ImGui::Begin("Entity List", &open))
		{
            ShowJsonEditor("../shared/");
		}


		ImGui::End();

		rlImGuiEnd();
		EndDrawing();
	}

    rlImGuiShutdown();
	CloseWindow(); 

	return 0;
}
