#include "game/register.h" // IWYU pragma: keep

#include "core/zeytin.h"
#include "core/guid/guid.h"

int main() {
    auto id = generateUniqueID();
    std::cout << id << std::endl;

    Zeytin::singleton().create_dummy(rttr::type::get_by_name("Position"));
}
