#pragma once
#include "Generic.h"

#if defined(WIN32)
#define __func__ __FUNCTION__
#endif

#define TAG_FUNCTION_NAME __func__

class Logger {

private:
    static void verror(fmt::string_view tag, fmt::string_view format, fmt::format_args args)
    {
        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::red), "[{}]: ", tag);
        fmt::vprint(format, args);
        fmt::print("\n");

        fmt::print(stderr, fmt::emphasis::bold | fmt::fg(fmt::color::red), "[{}]: ", tag);
        fmt::vprint(stderr, format, args);
        fmt::print(stderr, "\n");
    }

    static void vtrace(fmt::string_view tag, fmt::string_view format, fmt::format_args args)
    {
        fmt::print(fmt::fg(fmt::color::green), "[{}]: ", tag);
        fmt::vprint(format, args);
        fmt::print("\n");
    }

    static void vwarning(fmt::string_view tag, fmt::string_view format, fmt::format_args args)
    {
        fmt::print(fmt::fg(fmt::color::yellow), "[{}]: ", tag);
        fmt::vprint(format, args);
        fmt::print("\n");
    }

public:

    template <typename... T>
    static void terror(fmt::string_view tag, fmt::format_string<T...> format, T&&... args)
    {
        verror(tag, format, fmt::make_format_args(args...));
    }

    template <typename... T>
    static void ttrace(fmt::string_view tag, fmt::format_string<T...> format, T&&... args)
    {
        vtrace(tag, format, fmt::make_format_args(args...));
    }

    template <typename... T>
    static void twarning(fmt::string_view tag, fmt::format_string<T...> format, T&&... args)
    {
        vwarning(tag, format, fmt::make_format_args(args...));
    }

#define ERROR(tag, format, ...) terror(tag, format, __VA_ARGS__)
#define TRACE(tag, format, ...) ttrace(tag, format, __VA_ARGS__)
#define WARNING(tag, format, ...) twarning(tag, format, __VA_ARGS__)

};