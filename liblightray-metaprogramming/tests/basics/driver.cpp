#include <tuple>
#include <type_traits>
#include <iostream>

#include <lightray/metaprogramming/overload.hpp>
#include <lightray/metaprogramming/function_pointer.hpp>
#include <lightray/metaprogramming/type_pack.hpp>
#include <lightray/metaprogramming/type.hpp>
#include <lightray/metaprogramming/cast.hpp>
#include <lightray/metaprogramming/offset_of.hpp>


struct S { void f() { std::cout << "Hello, member f!\n"; } };
void f(S&, int) { std::cout << "Hello, non-member f!\n"; }

auto main() -> int
{
    using namespace lightray::mtp;

    auto my_tuple = type_pack<int, float, double, long>.filter([]<typename T>{ return std::is_integral<T>{}; })
        .apply([]<typename... Ts>{ return type<std::tuple<Ts...>>; })(42, 1000);

    function_pointer member_func = &S::f;
    function_pointer non_member_func = &f;
    overload o = {member_func, non_member_func};
    S s;
    o(s);
    o(s, 1);
    o(S{});

    struct S {};
    struct T : private S {};
    decltype(auto) test = lightray::mtp::private_forward_cast<S>(T{});
    decltype(auto) test2 = lightray::mtp::private_forward_cast<T>(S{});
}
