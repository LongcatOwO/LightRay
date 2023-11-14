#pragma once

#include <cstdio>
#include <format>
#include <ostream>
#include <utility>

namespace lightray::debug
{
    template <typename... Args>
    void print(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
    {
        std::fputs(std::format(fmt, std::forward<Args>(args)...).c_str(), stream);
    }

    template <typename... Args>
    void print(std::format_string<Args...> fmt, Args&&... args)
    {
        print(stdout, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void print(std::ostream& os, std::format_string<Args...> fmt, Args&&... args)
    {
        os << std::format(fmt, std::forward<Args>(args)...);
    }


    template <typename... Args>
    void println(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args)
    {
        print(stream, "{}\n", std::format(fmt, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void println(std::format_string<Args...> fmt, Args&&... args)
    {
        println(stdout, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void println(std::ostream& os, std::format_string<Args...> fmt, Args&&... args)
    {
        print(os, "{}\n", std::format(fmt, std::forward<Args>(args)...));
    }


} // namespace lightray::debug
