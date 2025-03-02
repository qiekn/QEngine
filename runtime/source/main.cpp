#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

// Function to iterate over the nested "value" object (if any)
void IterateValue(const rapidjson::Value& value, int indent = 0) {
    std::string indentation(indent, ' ');  // Creating indentation based on the level of recursion

    if (value.IsObject()) {
        for (auto& member : value.GetObject()) {
            std::cout << indentation << "Key: " << member.name.GetString() << " -> ";
            IterateValue(member.value, indent + 2);  // Recursive call with increased indentation
        }
    } 
    else if (value.IsArray()) {
        for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
            std::cout << indentation << "Array element " << i << " -> ";
            IterateValue(value[i], indent + 2);  // Recursive call with increased indentation
        }
    } 
    else if (value.IsString()) {
        std::cout << "String: " << value.GetString() << std::endl;
    }
    else if (value.IsInt()) {
        std::cout << "Integer: " << value.GetInt() << std::endl;
    }
    else if (value.IsDouble()) {
        std::cout << "Double: " << value.GetDouble() << std::endl;
    }
    else if (value.IsBool()) {
        std::cout << "Boolean: " << value.GetBool() << std::endl;
    }
    else if (value.IsNull()) {
        std::cout << "Null value" << std::endl;
    }
}

int main() {
    const char* json = R"({
        "entity_id": 11580387886217454472,
        "variants": [
            {
                "type": "Position",
                "value": {
                    "x": 0,
                    "y": 0
                }
            },
            {
                "type": "Player",
                "value": {
                    "position": {
                        "x": 0,
                        "y": 0
                    },
                    "velocity": {
                        "x": 0,
                        "y": 0
                    }
                }
            },
            {
                "type": "Velocity",
                "value": {
                    "x": 0,
                    "y": 0
                }
            }
        ]
    })";

    rapidjson::Document document;
    document.SetObject();
    document.Parse(json);

    if (document.HasParseError()) {
        std::cerr << "Error parsing JSON!" << std::endl;
        return 1;
    }

    std::cout << "Key: entity_id -> Integer: " << document["entity_id"].GetInt64() << std::endl;

    const rapidjson::Value& variants = document["variants"];
    for (rapidjson::SizeType i = 0; i < variants.Size(); ++i) {
        const rapidjson::Value& variant = variants[i];
        
        std::cout << "Array element " << i << " -> Key: type -> String: " << variant["type"].GetString() << std::endl;
        std::cout << "Key: value -> " << std::endl;

        IterateValue(variant["value"], 2);  // Start iterating the "value" with indentation
    }

    return 0;
}

