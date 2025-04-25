#include "variant/variant_list.h"

#include <filesystem>

#include "file_watcher/file_w.h"
#include "logger.h"

#include "path_resolver/path_resolver.h"

VariantList::VariantList() : m_variant_watcher(PathResolver::get().get_variant_folder(), std::chrono::milliseconds(500)) {
    load_variants();
    start_watching();
}

void VariantList::load_variants() {
    m_variants.clear();

    const auto& variant_folder = PathResolver::get().get_variant_folder();

    for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(variant_folder)) {
        if(!entry.is_regular_file() || entry.path().extension() != ".variant") {
            continue;
        }

        std::filesystem::path file_path = entry.path();
        std::string name = file_path.stem().string();

        m_variants.emplace_back<VariantDocument>(std::move(name));
    }

    for(auto& variant : m_variants) {
        variant.load_from_file();
    }
}

void VariantList::load_variant(const std::filesystem::path& path) {
    log_trace() << "Loading variant from path: " << path << std::endl;
    std::string name = path.stem().string();

    auto it = std::find_if(m_variants.begin(), m_variants.end(),
                          [&name](const auto& variant) {
                              return variant.get_name() == name;
                          });

    if (it != m_variants.end()) {
        log_trace() << "Variant already exist: " << path << std::endl;
        it->set_alive();
        it->load_from_file();
    } else {
        auto& rv = m_variants.emplace_back(VariantDocument(std::move(name)));
        rv.load_from_file();
    }
}

void VariantList::start_watching() {
    m_variant_watcher.add_callback({ ".variant"}, [this](const fs::path& path, const std::string& status) {
        if(status == "modified" || status == "created") {
            load_variant(path);
        }
        else if(status == "deleted") {
            log_trace() << "Deleting variant: " << path << std::endl;
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


