#include <iostream>
#include <format>

#include <lightray/metaprogramming/tuple_util.hpp>
#include <lightray/metaprogramming/value.hpp>
#include <lightray/reflection/dyn.hpp>
#include <lightray/reflection/gen_meta.hpp>
#include <lightray/reflection/type_info.hpp>

namespace np
{
    struct attrib : lightray::refl::attribute<lightray::refl::meta_category::type>
    {
    private:
        const char* _message;

    public:
        constexpr attrib(const char *m) : _message(m) {}
        constexpr const char* message() const noexcept { return _message; }
    };

    struct SBase
    {
    };

    template <typename T>
    struct S : SBase
    {
        int i;
        float f;
        T t;

        template <typename U>
        void func()
        {
            std::cout << "T: " << typeid(T).name() << " U: " << typeid(U).name() << '\n';
        }

        template <typename U>
        struct nested
        {
            U u;

            LIGHTRAY_REFL_TYPE(outer_class(S), nested, (),
                (var, u, ())
            )
        };

        LIGHTRAY_REFL_TYPE(namespace(np), S, (attributes(attrib{"Hi I'm attribute!"}), bases(SBase)), 
            (var, i, ())
            (var, f, ())
            (var, t, ())
            (func, func, ())
            (type, nested, ())
        )

    }; // struct S


    struct Prototype
    {
        void f();

        LIGHTRAY_REFL_TYPE(namespace(np), Prototype, (),
            (func, f, (id_accessor, interface_proxy((void)()())))
        )

    }; // struct Prototype

    struct Impl0
    {
        int value;
        void f() { std::puts(std::format("Impl0::f(): value = {}\n", value).c_str()); }
    };

    struct Impl1
    {
        int value;
        void f() { std::puts(std::format("Impl1::f(): 2xvalue = {}\n", value*2).c_str()); }
    };

} // namespace np


auto main() -> int
{
    using namespace lightray::refl;
    using namespace lightray::mtp;
    using namespace lightray::mtp::fixed_string_literals;
    using namespace lightray::mtp::splice::type;

    np::S<const char*> s{{}, 42, 3.14f, "hello, world!"};

    auto info = get_type_info(s);
    auto members = info.members();

    members.find_if([]<auto M>{ return value<M.name() == "func"_fs>; }).invoke<double>(s);
    members
    .filter([]<auto M>{ return value<M.category() == meta_category::variable>; })
    .for_each([&]<auto M>{ std::cout << M.name() << ": " << M.invoke(s) << '\n'; });

    auto t_type = members.find_if([]<auto M>{ return value<M.name() == "t"_fs>; }).type();
    auto attribs = info.attributes();
    auto attr = tuple_find_if(attribs, []<std::size_t I>{ return value<std::same_as<np::attrib, std::tuple_element_t<I, decltype(attribs)>>>; });
    std::cout << "attribute message: " << attr.message() << '\n';

    auto bases = info.bases();
    std::cout << typeid(decl_t<bases.get<0>()>).name() << '\n';

    auto nested_inst = info.members().find_if([]<auto M>{ return value<M.name() == "nested"_fs>; }).invoke<char>(none, 1);

    dyn<np::Prototype> d = std::make_unique<np::Impl0>(42);
    dyn<np::Prototype> d2 = std::make_unique<np::Impl1>(123);

    std::puts("d output: ");
    d.f();
    std::puts("d2 output: ");
    d2.f();
}
