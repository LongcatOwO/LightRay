#include <iostream>
#include <lightray/gen_meta.hpp>
#include <lightray/reflection.hpp>


namespace test
{
    template <typename TT>
    struct Outer
    {
        template <typename T>
        struct Inner;
    };

    template <typename T>
    struct Test
    {
        Test(T t) : var2(t) {}
    private:
        LIGHTRAY_REFL_DECL_FRIEND_TMPL(Test, PARAM_UNNAMED((typename)))
        using type = int;
        
        template <typename U>
        using type2 = U;

        constexpr static T var = 4.7;
        T var2;

        template <typename U>
        constexpr static U var3{5};

        auto add(T a, T b) { return a + b ;}

        template <typename U>
        auto print(U u) { std::cout << "print: " << u << '\n'; }

        template <typename U>
        static auto is_char() { std::cout << "is_char: " << std::is_same_v<char, U> << '\n'; }

        template <auto V>
        void print_constexpr() { std::cout << "print_constexpr: " << V << '\n'; }
    };

    LIGHTRAY_REFL_TYPE_TMPL(Test, PARAM((typename, T)), NAMESPACE(test), EXTRA(), MEMBERS(
        (TYPEDEF        type,                               EXTRA())
        (TYPEDEF_TMPL   type2,  PARAM((typename, U)),       EXTRA())
        (VAR            var,                                EXTRA())
        (VAR            var2,                               EXTRA())
        (VAR_TMPL       var3,   PARAM((typename, U)),       EXTRA())
        (FUNC           add,    OVERLOAD(NON_STATIC()),     EXTRA())
        (FUNC           print,  OVERLOAD(NON_STATIC()),     EXTRA())

        (FUNC           is_char, OVERLOAD(
            STATIC_TMPL         (PARAM((typename, U))), 
            NON_STATIC_TMPL     (PARAM((typename, U)))
        ), EXTRA())

        (FUNC print_constexpr, OVERLOAD(NON_STATIC_TMPL(PARAM((auto, V)))), EXTRA())
    ))
}



auto main() -> int
{
    using members = lightray::type_info<test::Test<float>>::get_all_members<>;
    using type = members::get_by_name<"type">::type;
    using type2 = members::get_by_name<"type2">::type<char>;
    
    constexpr auto var = members::get_by_name<"var">::invoke(nullptr);
    constexpr auto var3 = members::get_by_name<"var3">::invoke<long>(nullptr);
    constexpr auto var3_ptr = members::get_by_name<"var3">::pointer<long>;
    using var3_t = members::get_by_name<"var3">::type<long>;

    test::Test<float> t{1.3};
    members::for_each([](auto member) { std::cout << member.name << '\n'; });
    std::cout << members::get_by_name<"var">::invoke(t) << '\n';
    std::cout << members::get_by_name<"var2">::invoke(t) << '\n';
    std::cout << members::get_by_name<"var3">::invoke<char>(t) << '\n';

    return 0;
}
