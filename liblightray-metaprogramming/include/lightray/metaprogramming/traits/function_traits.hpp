#pragma once

#include <cstddef>
#include <type_traits>

#include <lightray/metaprogramming/type_pack.hpp>

#include "qualifier_traits.hpp"

namespace lightray::mtp::traits
{
    /*
     * Provides functional traits for any type that is "function-like".
     * This includes:
     *  - function signature
     *  - function reference
     *  - function pointer
     *  - member function pointer
     *  - functor with only one non-template overload for operator()
     *
     * This trait class should work for every possible function signatures
     * including cv-ref-noexcept qualifiers, and C-variadic arguments
     * (please don't actually use C-variadic arguments, it is unsafe).
     *
     * Here are the comprehensive list of the traits this class provides:
     *  - is_function_like: bool:
     *
     *      true if the type is "function-like" as described above.
     *      otherwise, false.
     *
     *      The rest of the traits are only defined if 
     *          is_function_like = true;
     *
     * From here on, the template type argument will be referred to
     * as "Function" instead for the sake of describing the rest of
     * the traits.
     *
     *  - return_type: typename: The return type of the Function
     *
     *  - argument_type<I>: typename: 
     *
     *      The type of the I-th argument in the Function. 
     *      This does not include the implicit this parameter in the case of member function pointer.
     *
     *  - argument_pack: type_pack_t: 
     *
     *      type_pack containing the argument_types of the Function.
     *      This does not include the implicit this parameter in the case of member function pointer.
     *
     *  - argument_count: std::size_t:
     *
     *      The number of arguments the Function accepts.
     *      This does not include the implicit this parameter in the case of member function pointer.
     *
     *  - signature_type: typename:
     *
     *      The pure function signature of the Function. 
     *
     *      For example,
     *          if Function = int (C::*)(const char*) const && noexcept
     *          then, signature_type = int (const char*) const && noexcept
     *
     *  - qualifier_traits: typename:
     *
     *      the trait class qualifier_traits of the Function's method signature qualifier. 
     *
     *      For example,
     *          if Function = int (C::*)(const char*) const && noexcept
     *          then, qualifier_traits = qualifier_traits<int const &&>.
     *
     *      Note that the 'int' in the qualifier_traits parameter is irrelevant, and is simply used as
     *      a carrier for the qualifiers. 
     *
     *      For more information, please see the trait class qualifier_traits.
     *
     *  - is_noexcept: bool: true if Function is noexcept, otherwise false.
     *  - is_variadic: bool: true if Function has C-variadic arguments, otherwise false.
     *
     *  Author: P. Lutchanont
     */
    template <typename Func>
    struct function_traits 
    {
        static constexpr auto is_function_like = false;

    }; // function_traits

    template <typename Func> requires std::is_function_v<Func>
    struct function_traits<Func*> : function_traits<Func> {};

    template <typename Func> requires std::is_function_v<Func>
    struct function_traits<Func&> : function_traits<Func> {};

    template <typename Func, typename C> requires std::is_function_v<Func>
    struct function_traits<Func C::*> : function_traits<Func> {};

    template <typename Func> requires requires { &Func::operator(); }
    struct function_traits<Func> : function_traits<decltype(&Func::operator())> {};

    template <typename R, typename... Args>
    struct function_traits<R (Args...)>
    {
        static constexpr auto is_function_like = true;

        using return_type = R;
        
        template <std::size_t I>
        using argument_type = typename decltype(type_pack<Args...>.template get<I>())::type;

        static constexpr auto argument_pack = type_pack<Args...>;

        static constexpr std::size_t argument_count = sizeof...(Args);

        using signature_type = R (Args...);
        using qualifier_traits = qualifier_traits<int>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;

    }; // function_traits<R (Args...)>

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const;
        using qualifier_traits = qualifier_traits<int const>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) volatile> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) volatile;
        using qualifier_traits = qualifier_traits<int volatile>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const volatile> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const volatile;
        using qualifier_traits = qualifier_traits<int const volatile>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) &> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) &;
        using qualifier_traits = qualifier_traits<int &>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const &> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const &;
        using qualifier_traits = qualifier_traits<int const &>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) volatile &> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) volatile &;
        using qualifier_traits = qualifier_traits<int volatile &>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const volatile &> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const volatile &;
        using qualifier_traits = qualifier_traits<int const volatile &>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) &&> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) &&;
        using qualifier_traits = qualifier_traits<int &&>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const &&> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const &&;
        using qualifier_traits = qualifier_traits<int const &&>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) volatile &&> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) volatile &&;
        using qualifier_traits = qualifier_traits<int volatile &&>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const volatile &&> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const volatile &&;
        using qualifier_traits = qualifier_traits<int const volatile &&>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = false;
    };

    
    

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...)> : function_traits<R (Args...)>
    {
        using signature_type = R (Args..., ...);
        using qualifier_traits = qualifier_traits<int>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const;
        using qualifier_traits = qualifier_traits<int const>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) volatile> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) volatile;
        using qualifier_traits = qualifier_traits<int volatile>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const volatile> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const volatile;
        using qualifier_traits = qualifier_traits<int const volatile>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) &> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) &;
        using qualifier_traits = qualifier_traits<int &>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const &> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const &;
        using qualifier_traits = qualifier_traits<int const &>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) volatile &> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) volatile &;
        using qualifier_traits = qualifier_traits<int volatile &>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const volatile &> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const volatile &;
        using qualifier_traits = qualifier_traits<int const volatile &>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) &&> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) &&;
        using qualifier_traits = qualifier_traits<int &&>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const &&> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const &&;
        using qualifier_traits = qualifier_traits<int const &&>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) volatile &&> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) volatile &&;
        using qualifier_traits = qualifier_traits<int volatile &&>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const volatile &&> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const volatile &&;
        using qualifier_traits = qualifier_traits<int const volatile &&>;
        static constexpr auto is_noexcept = false;
        static constexpr auto is_variadic = true;
    };








    template <typename R, typename... Args>
    struct function_traits<R (Args...) noexcept> : function_traits<R (Args...)>
    {
        using signature_type = R (Args...) noexcept;
        using qualifier_traits = qualifier_traits<int>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const noexcept;
        using qualifier_traits = qualifier_traits<int const>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) volatile noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) volatile noexcept;
        using qualifier_traits = qualifier_traits<int volatile>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const volatile noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const volatile noexcept;
        using qualifier_traits = qualifier_traits<int const volatile>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) & noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) & noexcept;
        using qualifier_traits = qualifier_traits<int &>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const & noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const & noexcept;
        using qualifier_traits = qualifier_traits<int const &>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) volatile & noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) volatile & noexcept;
        using qualifier_traits = qualifier_traits<int volatile &>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const volatile & noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const volatile & noexcept;
        using qualifier_traits = qualifier_traits<int const volatile &>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) && noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) && noexcept;
        using qualifier_traits = qualifier_traits<int &&>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const && noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const && noexcept;
        using qualifier_traits = qualifier_traits<int const &&>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) volatile && noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) volatile && noexcept;
        using qualifier_traits = qualifier_traits<int volatile &&>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args...) const volatile && noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args...) const volatile && noexcept;
        using qualifier_traits = qualifier_traits<int const volatile &&>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = false;
    };

    
    

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) noexcept> : function_traits<R (Args...)>
    {
        using signature_type = R (Args..., ...) noexcept;
        using qualifier_traits = qualifier_traits<int>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const noexcept;
        using qualifier_traits = qualifier_traits<int const>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) volatile noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) volatile noexcept;
        using qualifier_traits = qualifier_traits<int volatile>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const volatile noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const volatile noexcept;
        using qualifier_traits = qualifier_traits<int const volatile>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) & noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) & noexcept;
        using qualifier_traits = qualifier_traits<int &>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const & noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const & noexcept;
        using qualifier_traits = qualifier_traits<int const &>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) volatile & noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) volatile & noexcept;
        using qualifier_traits = qualifier_traits<int volatile &>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const volatile & noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const volatile & noexcept;
        using qualifier_traits = qualifier_traits<int const volatile &>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) && noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) && noexcept;
        using qualifier_traits = qualifier_traits<int &&>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const && noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const && noexcept;
        using qualifier_traits = qualifier_traits<int const &&>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) volatile && noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) volatile && noexcept;
        using qualifier_traits = qualifier_traits<int volatile &&>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

    template <typename R, typename... Args>
    struct function_traits<R (Args..., ...) const volatile && noexcept> : function_traits<R (Args...)> 
    {
        using signature_type = R (Args..., ...) const volatile && noexcept;
        using qualifier_traits = qualifier_traits<int const volatile &&>;
        static constexpr auto is_noexcept = true;
        static constexpr auto is_variadic = true;
    };

} // namespace lightray::mtp::traits
