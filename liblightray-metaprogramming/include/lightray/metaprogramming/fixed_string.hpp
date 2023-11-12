#pragma once

#include <array>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

namespace lightray::mtp
{
    template <typename CharT, std::size_t Len, typename Traits = std::char_traits<CharT>>
    struct basic_fixed_string
    {
        using container_type            = std::array<CharT, Len + 1>;
        using traits_type               = Traits;

        using value_type                = typename container_type::value_type;
        using reference                 = typename container_type::reference;
        using const_reference           = typename container_type::const_reference;
        using pointer                   = typename container_type::pointer;
        using const_pointer             = typename container_type::const_pointer;
        using difference_type           = typename container_type::difference_type;
        using size_type                 = typename container_type::size_type;
        using iterator                  = typename container_type::iterator;
        using const_iterator            = typename container_type::const_iterator;
        using reverse_iterator          = typename container_type::reverse_iterator;
        using const_reverse_iterator    = typename container_type::const_reverse_iterator;

        std::array<CharT, Len + 1> _data;

        constexpr basic_fixed_string() noexcept : _data{} {}

        constexpr basic_fixed_string(std::span<std::add_const_t<CharT>, Len> str) noexcept
        {
            std::copy(str.begin(), str.end(), _data.begin());
            _data[Len] = 0;
        }

        constexpr basic_fixed_string(std::add_const_t<CharT> (&str)[Len + 1]) noexcept
        {
            std::copy(std::begin(str), std::end(str), _data.begin());
            _data[Len] = 0;
        }

        constexpr auto operator=(std::span<CharT, Len> str) noexcept -> basic_fixed_string&
        {
            std::copy(str.begin(), str.end(), _data.begin());
            _data[Len] = 0;
            return *this;
        }

        constexpr auto operator=(CharT (&str)[Len + 1]) noexcept -> basic_fixed_string&
        {
            std::copy(std::begin(str), std::end(str), _data.begin());
            _data[Len] = 0;
            return *this;
        }

        constexpr auto at(size_type pos) -> reference
        {
            return _data.at(pos);
        }

        constexpr auto at(size_type pos) const -> const_reference
        {
            return _data.at(pos);
        }

        constexpr auto operator[](size_type pos) noexcept -> reference
        {
            return _data[pos];
        }

        constexpr auto operator[](size_type pos) const noexcept -> const_reference
        {
            return _data[pos];
        }

        constexpr auto front() noexcept -> reference
        {
            return _data.front();
        }

        constexpr auto front() const noexcept -> const_reference
        {
            return _data.front();
        }

        constexpr auto back() noexcept -> reference
        {
            return _data.back();
        }

        constexpr auto back() const noexcept -> const_reference
        {
            return _data.back();
        }

        constexpr auto data() noexcept -> pointer
        {
            return _data.data();
        }

        constexpr auto c_str() const noexcept -> const_pointer
        {
            return _data.data();
        }

        constexpr auto view() const noexcept
        {
            return std::basic_string_view<CharT, Traits>{_data.begin(), _data.end()};
        }

        constexpr operator std::basic_string_view<CharT, Traits>() const noexcept
        {
            return view();
        }

        constexpr auto begin() noexcept -> iterator
        {
            return _data.begin();
        }

        constexpr auto begin() const noexcept -> const_iterator
        {
            return _data.begin();
        }

        constexpr auto cbegin() const noexcept -> const_iterator
        {
            return _data.cbegin();
        }

        constexpr auto end() noexcept -> iterator
        {
            return _data.end();
        }

        constexpr auto end() const noexcept -> const_iterator
        {
            return _data.end();
        }

        constexpr auto cend() const noexcept -> const_iterator
        {
            return _data.cend();
        }

        constexpr auto rbegin() noexcept -> reverse_iterator
        {
            return _data.rbegin();
        }

        constexpr auto rbegin() const noexcept -> const_reverse_iterator
        {
            return _data.rbegin();
        }

        constexpr auto crbegin() const noexcept -> const_reverse_iterator
        {
            return _data.crbegin();
        }

        constexpr auto rend() noexcept -> reverse_iterator
        {
            return _data.rend();
        }

        constexpr auto rend() const noexcept -> const_reverse_iterator
        {
            return _data.rend();
        }

        constexpr auto crend() const noexcept -> const_reverse_iterator
        {
            return _data.crend();
        }

        constexpr auto empty() const noexcept -> bool
        {
            return _data.empty();
        }

        constexpr auto size() const noexcept -> size_type
        {
            return _data.size();
        }

        constexpr auto length() const noexcept -> size_type
        {
            return size();
        }

        template <std::size_t OtherLen>
        constexpr auto compare(const basic_fixed_string<CharT, OtherLen, Traits>& other) const noexcept
        -> int
        {
            return this->view().compare(other.view());
        }

        template <std::size_t OtherLen>
        constexpr auto operator==(const basic_fixed_string<CharT, OtherLen, Traits>& other) const noexcept
        -> bool
        {
            return this->view() == other.view();
        }

        template <std::size_t OtherLen>
        constexpr auto operator<=>(const basic_fixed_string<CharT, OtherLen, Traits>& other) const noexcept
        -> decltype(auto)
        {
            return this->view() <=> other.view();
        }

    }; // struct basic_fixed_string

    template <typename CharT, std::size_t Len>
    basic_fixed_string(CharT (&)[Len]) -> basic_fixed_string<std::remove_const_t<CharT>, Len - 1>;

    template <typename CharT, std::size_t Len, typename Traits>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const basic_fixed_string<CharT, Len, Traits>& s)
    {
        return os << s.view();
    }


    template <std::size_t Len>
    struct fixed_string : basic_fixed_string<char, Len>
    {
    private:
        using base = basic_fixed_string<char, Len>;

    public:
        using base::base;
        using base::operator=;

    }; // struct fixed_string

    template <std::size_t Len>
    fixed_string(const char (&)[Len]) -> fixed_string<Len - 1>;


    template <std::size_t Len>
    struct wfixed_string : basic_fixed_string<wchar_t, Len>
    {
    private:
        using base = basic_fixed_string<wchar_t, Len>;

    public:
        using base::base;
        using base::operator=;

    }; // struct wfixed_string

    template <std::size_t Len>
    wfixed_string(const wchar_t (&)[Len]) -> wfixed_string<Len - 1>;

    template <std::size_t Len>
    struct u16fixed_string : basic_fixed_string<char16_t, Len>
    {
    private:
        using base = basic_fixed_string<char16_t, Len>;

    public:
        using base::base;
        using base::operator=;

    }; // struct u16fixed_string

    template <std::size_t Len>
    u16fixed_string(const char16_t (&)[Len]) -> u16fixed_string<Len - 1>;


    template <std::size_t Len>
    struct u32fixed_string : basic_fixed_string<char32_t, Len>
    {
    private:
        using base = basic_fixed_string<char32_t, Len>;

    public:
        using base::base;
        using base::operator=;

    }; // struct u32fixed_string

    template <std::size_t Len>
    u32fixed_string(const char32_t (&)[Len]) -> u32fixed_string<Len - 1>;

    namespace fixed_string_literals
    {
        template <basic_fixed_string Str>
        constexpr auto operator ""_fs() noexcept
        {
            return Str;
        }

    } // namespace fixed_string_literals

} // namespace lightray::mtp
