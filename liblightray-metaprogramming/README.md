# liblightray-metaprogramming

A C++ template metaprogramming library made for the project LightRay.

This library aims to provide useful tools for performing generic template
metaprogramming. This includes various trait classes, compile-time manipulation classes,
and syntax unification classes.

## Examples

```c++
using namespace lightray::mtp;

// my_tuple is std::tuple<int, long>{42, 1000}
auto my_tuple = type_pack<int, float, double, long>.filter([]<typename T>{
    return std::is_integral<T>{};
}).apply([]<typename... Ts>{ return type<std::tuple<Ts...>>; })(42, 1000);
```

TODO: need more examples
