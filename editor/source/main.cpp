#include "raylib.h"

#include "imgui.h"
#include "rlImGui.h"

#include <iostream>
#include <filesystem>
#include <fstream>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include "rapidjson/prettywriter.h"
#include <imgui.h>

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

void ShowJsonEditor(const fs::path& filePath) {
    std::ifstream inFile(filePath);
    if (!inFile) {
        std::cerr << "Error: Unable to open file: " << filePath << std::endl;
        return;
    }

    std::string jsonString((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    rapidjson::Document document;
    document.Parse(jsonString.c_str());

    if (document.HasParseError()) {
        std::cerr << "Error parsing JSON: " << document.GetParseError() << std::endl;
        return;
    }

    if (ImGui::CollapsingHeader("Entity")) {
        const rapidjson::Value& variants = document["variants"];
        for (rapidjson::SizeType i = 0; i < variants.Size(); ++i) {
            const rapidjson::Value& variant = variants[i];
            const char* type = variant["type"].GetString();

            ImGui::Indent();
            if (ImGui::CollapsingHeader((std::string(type) + "##" + std::to_string(i)).c_str())) {
                const rapidjson::Value& value = variant["value"];
                for (auto& member : value.GetObject()) {
                    if (member.value.IsInt()) {
                        int intValue = member.value.GetInt();
                        std::string key = member.name.GetString();
                        
                        if (ImGui::InputInt(("Value: " + key + "##" + std::to_string(i)).c_str(), &intValue)) {
                            const_cast<rapidjson::Value&>(member.value).SetInt(intValue);
                            SaveJsonToFile(filePath, document);  // Save after every edit
                        }
                    }
                }
            }
            ImGui::Unindent();
        }
    }
}


float ScaleToDPIF(float value)
{
    return GetWindowScaleDPI().x * value;
}

int ScaleToDPII(int value)
{
    return int(GetWindowScaleDPI().x * value);
}

void DisplayJson(rapidjson::Value& jsonValue) {
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
            ShowJsonEditor("../shared/player.entity");
		}


		ImGui::End();

		rlImGuiEnd();
		EndDrawing();
	}

    rlImGuiShutdown();
	CloseWindow(); 

	return 0;
}
