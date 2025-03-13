#pragma once

#include <vector>
#include "variant_document.h"
#include "file_watcher/file_watcher.h"

class VariantList final {
public:
    VariantList();

    inline std::vector<VariantDocument>& get_variants() { return m_variants;}

private:
    void load_variants();
    void start_watching();
    void load_variant(const std::filesystem::path& path);


    std::vector<VariantDocument> m_variants;
    FileWatcher m_variant_watcher;
};
