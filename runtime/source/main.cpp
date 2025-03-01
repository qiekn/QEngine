#include "game/register.h" // IWYU pragma: keep

#include "core/zeytin.h"
#include "core/guid/guid.h"

int main() {
    auto id = generateUniqueID();
    std::cout << id << std::endl;
}
