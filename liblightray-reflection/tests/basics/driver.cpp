#include <iostream>

#include <lightray/metaprogramming/tuple_util.hpp>
#include <lightray/metaprogramming/value.hpp>
#include <lightray/reflection/gen_meta.hpp>
#include <lightray/reflection/type_info.hpp>

namespace np
{
    struct S
    {
        int i;
        float f;

        void func()
        {
            std::cout << "S::func()\n";
        }

        LIGHTRAY_REFL_TYPE(namespace(np), S, (), 
            (var, i, ())
            (var, f, ())
            (func, func, ())
        )

    }; // struct S

} // namespace np

auto main() -> int
{
    using namespace lightray::refl;
    using namespace lightray::mtp;
    using namespace lightray::mtp::fixed_string_literals;

    np::S s{42, 3.14f};

    auto info = type_info_<np::S>;
    auto members = info.members();

    members.find_if([]<auto M>{ return value<M.name() == "func"_fs>; }).invoke(s);
    members
    .filter([]<auto M>{ return value<M.category() == meta_category::variable>; })
    .for_each([&]<auto M>{ std::cout << M.name() << ": " << M.invoke(s) << '\n'; });

}
