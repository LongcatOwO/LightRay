# liblightray-reflection

Compile-time, (almost) type-complete reflection C++ library. Allows you to
inspect your type and performing operations on them reflectively with guaranteed
type safety.

## Examples

Defining a generic "print all data members" function.

```c++
#include <iostream>
#include <lightray/reflection/gen_meta.hpp>
#include <lightray/reflection/member_info.hpp>
#include <lightray/reflection/meta_category.hpp>
#include <lightray/reflection/type_info.hpp>

template <reflected T>
void print_all_data_members(const T& obj)
{
    using namespace lightray::refl;
    using namespace lightray::mtp;

    // filter members
    type_info_<T>.members().filter([]<auto Member>{
        // return compile-time constant
        // true if member is a non-static variable
        return value<
            (Member.category() == meta_category::variable)
         && (Member.is_static() == false_) // false_ is an alias for value<false>.
        >; 
    })
    // print each member
    .for_each([&]<auto Member>{
        std::cout << Member.name() << ": " << Member.invoke(obj) << '\n';
    });
}

struct S
{
    int integer_data;
    float float_data;

    // Add reflection data
    LIGHTRAY_REFL_TYPE(namespace(::), S, (),
        (var, integer_data, ())
        (var, float_data, ())
    )
};

// Reflection can work on template too.
template <typename T>
struct Vec3
{
    T x, y, z;

    // Add reflection data
    LIGHTRAY_REFL_TYPE(namespace(::), Vec3, (),
        (var, x, ())
        (var, y, ())
        (var, z, ())
    )
};

int main()
{
    S s = {42, 3.14f};
    Vec3<double> dvec = { 2.3, 3.4, 4.5 };
    print_all_data_members(s); // output: integer_data: 42
                               //         float_data: 3.14
    print_all_data_members(dvec); // output: x: 2.3
                                  //         y: 3.4
                                  //         y: 4.5
}
```

Automatic implementation of ducked-type type erased object.

```c++
#include <memory>
#include <string>
#include <vector>
#include <lightray/reflection/dyn.hpp>
#include <lightray/reflection/gen_meta.hpp>

struct Stringifiable
{
    int to_string; // create a dummy variable just for its id
                   // its type does not matter

    LIGHTRAY_REFL_TYPE(namespace(::), Stringifiable, (),
        // enable id_accessor and define the proxy function signature as
        // 'std::string to_string()'
        // aka. a function which takes no parameter and output std::string.
        (var, to_string, (id_accessor, interface_proxy((std::string)()(const))))
    )
};

struct Cat { std::string to_string() const { return "Cat"; } };
struct Dog { std::string to_string() const { return "Dog"; } };

int main()
{
    using namespace lightray::refl;

    std::vector<dyn<Stringifiable>> stringifiables;
    stringifiables.push_back(std::make_unique<Cat>());
    stringifiables.push_back(std::make_unique<Dog>());

    // output:
    // Cat
    // Dog
    for (const auto& e : stringifiables)
        std::cout << e.to_string() << '\n';
}
```
