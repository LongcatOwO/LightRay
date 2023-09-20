#ifndef _LIGHTRAY_COREDEF_HPP_
#define _LIGHTRAY_COREDEF_HPP_


#include <type_traits>


namespace lightray
{
    struct category
    {
        using underlying_type = unsigned;
        underlying_type _value;

    private:
        constexpr category() noexcept : _value(0) {}
        constexpr category(underlying_type v) : _value(v) {}

    public:
        constexpr underlying_type value() const noexcept { return _value; }
        constexpr bool is_superset_of(category other) const noexcept { return (*this | other) == *this; }
        constexpr bool is_subset_of(category other) const noexcept { return (*this & other) == *this; }
        constexpr bool is_intersected_with(category other) const noexcept { return !!(*this & other).value(); }

        explicit constexpr operator underlying_type() const noexcept { return _value; }
        explicit constexpr operator bool() const noexcept { return !!_value; }
        constexpr category operator|(category other) const noexcept { return { _value | other._value }; }
        constexpr category operator&(category other) const noexcept { return { _value & other._value }; }
        constexpr category operator^(category other) const noexcept { return { _value ^ other._value }; }
        constexpr bool operator==(category const &other) const noexcept = default;
        constexpr bool operator<(category const &other) const noexcept = delete;
        constexpr bool operator>(category const &other) const noexcept = delete;

        static category const type;
        static category const type_template;
        static category const type_definition;
        static category const type_definition_template;
        static category const variable;
        static category const variable_template;
        static category const function;

        static category const all;
        static category const none;
    };

    constexpr category category::type                           = 1 << 0;
    constexpr category category::type_template                  = 1 << 1;
    constexpr category category::type_definition                = 1 << 2;
    constexpr category category::type_definition_template       = 1 << 3;
    constexpr category category::variable                       = 1 << 4;
    constexpr category category::variable_template              = 1 << 5;
    constexpr category category::function                       = 1 << 6;

    constexpr category category::all                            = 0 - 1;
    constexpr category category::none                           = 0;


    template <typename T>
    using meta = decltype(lightray_meta_get(std::declval<T>()));

    template <typename T>
    concept reflected = requires { typename meta<T>; };
}


#endif
