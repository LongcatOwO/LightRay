#pragma once

#include <type_traits>
#include <utility>

#include <lightray/metaprogramming/traits/function_traits.hpp>
#include <lightray/metaprogramming/traits/pointer_traits.hpp>


namespace lightray::mtp
{
    namespace detail
    {
        /*
         * Author: P. Lutchanont
         */
        template <typename FuncPtr>
        struct function_pointer_base
        {
            using function_traits = traits::function_traits<FuncPtr>;
            using qualifier_traits = typename function_traits::qualifier_traits;
            using pointer_traits = traits::pointer_traits<FuncPtr>;

        protected:
            FuncPtr _ptr;

        public:
            constexpr function_pointer_base() = default;
            constexpr function_pointer_base(FuncPtr fp) noexcept : _ptr(fp) {}

        }; // struct function_pointer_base

        template <typename FuncPtr, typename ISeq>
        struct function_pointer_indexer;

        /*
         * Specialization for regular function pointer.
         */
        template <typename FuncPtr, std::size_t... ArgIndices>
        requires 
        (
            std::is_function_v<typename traits::pointer_traits<FuncPtr>::element_type>
         && !std::is_member_pointer_v<FuncPtr>
        )
        struct function_pointer_indexer<FuncPtr, std::index_sequence<ArgIndices...>>
        : function_pointer_base<FuncPtr>
        {
        private:
            using base = function_pointer_base<FuncPtr>;

        public:
            using base::base;

            constexpr auto operator()(typename base::function_traits::template argument_type<ArgIndices>... args) const
            noexcept(noexcept(base::_ptr(std::forward<decltype(args)>(args)...)))
            -> typename base::function_traits::return_type
            {
                return base::_ptr(std::forward<decltype(args)>(args)...);
            }

        }; // struct function_pointer_indexer<FuncPtr, std::index_sequence<ArgIndices...>>

        /*
         * Specialization for member function pointer with no reference qualifier.
         */
        template <typename FuncPtr, std::size_t... ArgIndices>
        requires
        (
            std::is_function_v<typename traits::pointer_traits<FuncPtr>::element_type>
         && std::is_member_pointer_v<FuncPtr>
         && !traits::function_traits<FuncPtr>::qualifier_traits::is_reference
        )
        struct function_pointer_indexer<FuncPtr, std::index_sequence<ArgIndices...>>
        : function_pointer_base<FuncPtr>
        {
        private:
            using base = function_pointer_base<FuncPtr>;
            using cv_qualified_class_type = typename base::qualifier_traits::template apply_cv<
                typename base::pointer_traits::class_type
            >;

        public:
            using base::base;

            constexpr auto operator()(cv_qualified_class_type& self, typename base::function_traits::template argument_type<ArgIndices>... args) const
            noexcept(noexcept((self.*base::_ptr)(std::forward<decltype(args)>(args)...)))
            -> typename base::function_traits::return_type
            {
                return (self.*base::_ptr)(std::forward<decltype(args)>(args)...);
            }

            constexpr auto operator()(cv_qualified_class_type&& self, typename base::function_traits::template argument_type<ArgIndices>... args) const
            noexcept(noexcept((std::move(self).*base::_ptr)(std::forward<decltype(args)>(args)...)))
            -> typename base::function_traits::return_type
            {
                return (std::move(self).*base::_ptr)(std::forward<decltype(args)>(args)...);
            }

        }; // struct function_pointer_indexer<FuncPtr, std::index_sequence<ArgIndices...>>

        /*
         * Specialization for member function pointer with lvalue reference qualifier.
         */
        template <typename FuncPtr, std::size_t... ArgIndices>
        requires
        (
            std::is_function_v<typename traits::pointer_traits<FuncPtr>::element_type>
         && std::is_member_pointer_v<FuncPtr>
         && traits::function_traits<FuncPtr>::qualifier_traits::is_lvalue_reference
        )
        struct function_pointer_indexer<FuncPtr, std::index_sequence<ArgIndices...>>
        : function_pointer_base<FuncPtr>
        {
        private:
            using base = function_pointer_base<FuncPtr>;
            using cv_qualified_class_type = typename base::qualifier_traits::template apply_cv<
                typename base::pointer_traits::class_type
            >;

        public:
            using base::base;

            constexpr auto operator()(cv_qualified_class_type& self, typename base::function_traits::template argument_type<ArgIndices>... args) const
            noexcept(noexcept((self.*base::_ptr)(std::forward<decltype(args)>(args)...)))
            -> typename base::function_traits::return_type
            {
                return (self.*base::_ptr)(std::forward<decltype(args)>(args)...);
            }

        }; // struct function_pointer_indexer<FuncPtr, std::index_sequence<ArgIndices...>>

        /*
         * Specialization for member function pointer with rvalue reference qualifier.
         */
        template <typename FuncPtr, std::size_t... ArgIndices>
        requires
        (
            std::is_function_v<typename traits::pointer_traits<FuncPtr>::element_type>
         && std::is_member_pointer_v<FuncPtr>
         && traits::function_traits<FuncPtr>::qualifier_traits::is_rvalue_reference
        )
        struct function_pointer_indexer<FuncPtr, std::index_sequence<ArgIndices...>>
        : function_pointer_base<FuncPtr>
        {
        private:
            using base = function_pointer_base<FuncPtr>;
            using cv_qualified_class_type = typename base::qualifier_traits::template apply_cv<
                typename base::pointer_traits::class_type
            >;

        public:
            using base::base;

            constexpr auto operator()(cv_qualified_class_type&& self, typename base::function_traits::template argument_type<ArgIndices>... args) const
            noexcept(noexcept((std::move(self).*base::_ptr)(std::forward<decltype(args)>(args)...)))
            -> typename base::function_traits::return_type
            {
                return (std::move(self).*base::_ptr)(std::forward<decltype(args)>(args)...);
            }

        }; // struct function_pointer_indexer<FuncPtr, std::index_sequence<ArgIndices...>>

    } // namespace detail

    /*
     * Acts as a wrapper over function pointer or member function pointer to provide
     * a unified function calling syntax with explicit 'this' object parameter
     * in this case of member function pointer.
     *
     * This class is also useful for providing operator() for (member) function pointers
     * allowing them to be used in combination with the class 'overloaded'.
     *
     * As the operator() of this class is not templated, using this class with 'overloaded' 
     * does not cause function call ambiguity issues that perfect forwarding functors usually have.
     *
     * For regular function pointer, operator() has exactly the same calling signature as
     * the function pointer itself.
     *
     * For member function pointer, operator() has the 'this' object as its first parameter.
     * The type of 'this' object is a cv-ref qualified type of the owning class type.
     * The 'this' object qualifier follows the invocation lookup rules for qualified
     * member method.
     *
     * For example:
     *
     *  If FuncPtr = void (C::*)(int, char), then these operator() overloads are defined:
     *      void operator()(C&, int, char)
     *      void operator()(C&&, int char)
     *
     *  If FuncPtr = const char* (C::*)(float&) const &&, then this operator() overload is defined:
     *      const char* operator()(C const &&, float&)
     *
     *
     * Author: P. Lutchanont
     */
    template <typename FuncPtr>
    requires std::is_function_v<typename traits::pointer_traits<FuncPtr>::element_type>
    struct function_pointer
    :   detail::function_pointer_indexer<
            FuncPtr, 
            std::make_index_sequence<traits::function_traits<FuncPtr>::argument_count>
        >
    {
    private:
        using base = detail::function_pointer_indexer<
            FuncPtr,
            std::make_index_sequence<traits::function_traits<FuncPtr>::argument_count>
        >;

    public:
        using base::base;
        using base::operator();

    }; // struct function_pointer

    // deduce by value.
    template <typename Fn>
    function_pointer(Fn) -> function_pointer<Fn>;

} // lightray::mtp
