#include "variant/variant_list.h"

#include <filesystem>
#include <iostream>

#include "file_watcher/file_watcher.h"

VariantList::VariantList() : m_variant_watcher("../shared/variants/", std::chrono::milliseconds(500)) {
    load_variants();
    start_watching();
}

void VariantList::load_variants() {
    m_variants.clear();

    for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(VARIANT_FOLDER)) {
        if(!entry.is_regular_file() || entry.path().extension() != ".variant") {
            continue;
        }

        std::filesystem::path file_path = entry.path();
        std::string file_name = file_path.stem().string();

        m_variants.emplace_back<VariantDocument>(std::move(file_name));
    }

    // NOTE: this could run parelel
    for(auto& variant : m_variants) {

        variant.load_from_file();
    }
}
void VariantList::load_variant(const std::filesystem::path& path) {
    std::string name = path.stem().string();
    auto& rv = m_variants.emplace_back<VariantDocument>(std::move(name));
    rv.load_from_file();
}

void VariantList::start_watching() {
    m_variant_watcher.addCallback({ ".variant"}, [this](const fs::path& path, const std::string& status) {
        std::cout << path << " is " << status << std::endl;
        if(status == "modified" || status == "created") {
            load_variant(path);
        }
        else if(status == "deleted") {
            std::string name = path.stem().string();
            for(auto& variant : m_variants) {
                if(variant.get_name() == name) {
                    variant.mark_dead();
                }
            }
        }
    });
    
    std::thread watcher_thread([this]() {
        m_variant_watcher.start();
    });

    watcher_thread.detach(); 
}


