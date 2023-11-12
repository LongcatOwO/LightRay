#include <iostream>

#include <lightray/metaprogramming/tuple_util.hpp>
#include <lightray/metaprogramming/value.hpp>
#include <lightray/reflection/gen_meta.hpp>
#include <lightray/reflection/type_info.hpp>

namespace np
{
    template <typename T>
    struct S
    {
        int i;
        float f;
        T t;

        template <typename U>
        void func()
        {
            std::cout << "T: " << typeid(T).name() << " U: " << typeid(U).name() << '\n';
        }

        LIGHTRAY_REFL_TYPE(namespace(np), S, (), 
            (var, i, ())
            (var, f, ())
            (var, t, ())
            (func, func, ())
        )

    }; // struct S

} // namespace np

auto main() -> int
{
    using namespace lightray::refl;
    using namespace lightray::mtp;
    using namespace lightray::mtp::fixed_string_literals;

    np::S<const char*> s{42, 3.14f, "hello, world!"};

    auto info = get_type_info(s);
    auto members = info.members();

    members.find_if([]<auto M>{ return value<M.name() == "func"_fs>; }).invoke<double>(s);
    members
    .filter([]<auto M>{ return value<M.category() == meta_category::variable>; })
    .for_each([&]<auto M>{ std::cout << M.name() << ": " << M.invoke(s) << '\n'; });

    auto t_type = members.find_if([]<auto M>{ return value<M.name() == "t"_fs>; }).type();

}
