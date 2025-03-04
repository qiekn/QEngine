#pragma once

#include <vector>
#include "variant_document.h"

class VariantList final {

public:
    void load_variants();

    inline std::vector<VariantDocument>& get_variants() { return m_variants;}

private:
    std::vector<VariantDocument> m_variants;
};
