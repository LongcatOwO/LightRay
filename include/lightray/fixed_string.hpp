#ifndef _LIGHTRAY_FIXED_STRING_HPP_INCLUDED_
#define _LIGHTRAY_FIXED_STRING_HPP_INCLUDED_

#include <algorithm>
#include <compare>
#include <cstddef>
#include <iterator>
#include <ostream>
#include <span>
#include <string_view>

namespace lightray
{
    /*
     * Precondition: last character in the buffer is always null
     */
    template <std::size_t Len>
    struct fixed_string
    {
        char _data[Len + 1];

        constexpr fixed_string() noexcept : _data{} {}

        constexpr fixed_string(char const *str) noexcept
        {
            auto it = std::begin(_data), last = std::end(_data) - 1;
            while (it != last && *str) *it++ = *str++;
            std::fill(it, std::end(_data), '\0');
        }

        constexpr fixed_string(std::string_view str) noexcept
        {
            auto it = std::begin(_data), last = std::end(_data) - 1;
            auto str_it = std::begin(str), str_end = std::end(str);
            while (it != last && str_it != str_end) *it++ = *str_it++;
            std::fill(it, std::end(_data), '\0');
        }

        template <std::size_t N>
        constexpr auto operator<=>(fixed_string<N> const &other) const noexcept
        {
            return std::string_view(_data) <=> std::string_view(other._data);
        }

        template <std::size_t N>
        constexpr auto operator==(fixed_string<N> const &other) const noexcept
        {
            return ((*this) <=> other) == 0;
        }

        template <std::size_t N>
        constexpr auto operator<(fixed_string<N> const &other) const noexcept
        {
            return ((*this) <=> other) < 0;
        }

        template <std::size_t N>
        constexpr auto operator>(fixed_string<N> const &other) const noexcept
        {
            return ((*this) <=> other) > 0;
        }

        constexpr auto operator<=>(std::string_view other) const noexcept
        {
            return std::string_view(_data) <=> other;
        }

        constexpr auto operator==(std::string_view other) const noexcept
        {
            return ((*this) <=> other) == 0;
        }

        constexpr auto operator<(std::string_view other) const noexcept
        {
            return ((*this) <=> other) < 0;
        }

        constexpr auto operator>(std::string_view other) const noexcept
        {
            return ((*this) <=> other) > 0;
        }
    };

    template <std::size_t StrLen>
    fixed_string(char const (&)[StrLen]) -> fixed_string<StrLen - 1>;

    namespace literal
    {
        template <fixed_string Fs>
        constexpr auto operator"" _fs() noexcept
        {
            return Fs;
        }

    }

    template <typename T>
    concept fixed_string_type = requires (T &&t)
    {
        []<std::size_t N>(fixed_string<N> const &){}(t);
    };

    template <typename CharT, typename Traits, std::size_t Len>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits> &os, fixed_string<Len> const &fs) noexcept
    {
        os << fs._data;
        return os;
    }

}

#endif
