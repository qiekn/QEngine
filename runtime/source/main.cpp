#include <iostream>
#include "rttr/registration.h"


struct MyStruct { MyStruct() {}; void func(double) {}; int data; };

RTTR_REGISTRATION
{
    rttr::registration::class_<MyStruct>("MyStruct")
         .constructor<>()
         .property("data", &MyStruct::data)
         .method("func", &MyStruct::func);
}

int main() {
    rttr::type t = rttr::type::get<MyStruct>();
    for (auto& prop : t.get_properties())
        std::cout << "name: " << prop.get_name();

    for (auto& meth : t.get_methods())
        std::cout << "name: " << meth.get_name();
}
