# LightRay

A collection of personally made general purpose C++ libraries/frameworks
that aims to cover a variety of use cases and provide useful utilities.

## Available Packages

### [Debug](./liblightray-debug)

Provides debugging utilities such as assertions, printing functions,
as well as, framework for testing.

### [Metaprogramming](./liblightray-metaprogramming)

Eases the pain of doing template metaprogramming/compile-time programming
with C++20 features like explicit template lambda, concepts, etc.

Some of the features are:

1. Functional and fluent interface for manipulating variadic type pack.
2. Automatic implementation of enum flag operations without macro.
3. Unified function pointer abstraction (including member function pointer)
4. Compile-time string.
5. Safer type casts.

and many more...

### [Preprocessor](./liblightray-preprocessor)

Contains tool for macro metaprogramming, such as for-loop, if, increment,
get i-th element from variadic argument, etc.

### [Reflection](./liblightray-reflection)

Enables type introspection features at compile-time using macro.
This library is built on top of the [Metaprogramming](./liblightray-metaprogramming)
library.

Current features include:

1. Completely type-safe reflection facilities.
2. Compile-time type information, such as members(type, data, function).
3. Compile-time member information, allowing construction(type), access(data), invocation(function)
4. The ability to iterate members through type information.
5. Automatic implementation of ducked-type type erased object with customizable interface.
6. Creating custom attributes, and retrieving them at compile-time.

## Disclaimer

All of these packages are subjected to breaking changes, and is not yet well-tested.
Therefore, please use them at your own risk. However, I still hope that you will
find them useful, interesting, and gives you a new way of thinking about C++.
