#include "variant/variant_list.h"

#include <filesystem>

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
