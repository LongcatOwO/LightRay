# LightRay.Preprocessor

A C++ preprocessor metaprogramming library made for the project LightRay.
This library contains a small subset of Boost.Preprocessor in terms of
functionality, that has been tweaked to match the need of this project.

If you need a well-tested preprocessor metaprogramming library with
that can cover a comprehensive list of use cases, Boost.Preprocessor is
probably what you should be using instead.

This library is nowhere near production quality, so please be aware.

## Motivation

I encountered Boost's preprocessor library a few years ago, and have been
inspired to try and implement them myself. Even though most use cases of
macros had already been replaced with a better alternatives in the world
of modern C++ (template, constexpr, compiler inline optimization), there
are still some dark corners where macros are unavoidable, reflection in
particular.

As C++ does not come with built-in reflection system (as of 2023), programmers
have to resort to various means to generate the reflection metadata themselves.

Macros, though considered ugly and obscured by many, is still, unfortunately,
one of the only reliable way to perform code generation in a standard-compliant
manner without any extra steps in your build toolchain.

This library is specifically made to complement LightRay.Reflection,
providing the needed utility to perform metadata generation. Note that however,
this library is intended to be completely generic and uncoupled from any other
libraries in the LightRay project, and can be used standalone.
