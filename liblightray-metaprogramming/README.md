# liblightray-metaprogramming

A C++ template metaprogramming library made for the project LightRay.

This library aims to provide useful tools for performing generic template
metaprogramming. This includes various trait classes, functional and fluent
interface for variadic type pack manipulation, and syntax unification classes.

## Examples

Performing type manipulation:

```c++
#include <tuple>
#include <type_traits>
#include <lightray/metaprogramming/type_pack.hpp>

int main()
{
    using namespace lightray::mtp;

    // my_tuple is std::tuple<int, long>{42, 1000}
    auto my_tuple = type_pack<int, float, double, long>.filter([]<typename T>{
        return std::is_integral<T>{};
    }).apply([]<typename... Ts>{ return type<std::tuple<Ts...>>; })(42, 1000);
}
```

Overloading member function with non-member function:

```c++
#include <iostream>
#include <lightray/metaprogramming/function_pointer.hpp>
#include <lightray/metaprogramming/overload.hpp>

// member function declaration
struct S { void f() { std::cout << "Hello, member f!\n"; } };

// non-member function declaration
void f(S&, int) { std::cout << "Hello, non-member f!\n"; }

int main()
{
    using namespace lightray::mtp;
    function_pointer member_func = &S::f;
    function_pointer non_member_func = &f;
    overload o = {member_func, non_member_func};
    S s;
    o(s); // this prints "Hello, member f!\n"
    o(s, 1); // this prints "Hello, non-member f!\n"
}
```
