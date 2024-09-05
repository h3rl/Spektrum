#pragma once
#include <fmt/color.h>
#include <utility> // for std::forward

// Variadic template for logging
template <typename... T>
inline void _log(const char *file, int line, const fmt::text_style st, fmt::string_view prefix,
                 fmt::format_string<T...> format, bool add_newline, T &&...args)
{
	if (file && line)
	{
		fmt::print(st, "{}{} ({}:{})", prefix, fmt::format(format, std::forward<T>(args)...), file, line);
	}
	else
	{
		fmt::print(st, "{}{}", prefix, fmt::format(format, std::forward<T>(args)...));
	}

	if (add_newline)
	{
		fmt::print("\n");
	}
}

// Macros for different log levels
#define log_msg(format, ...) _log(nullptr, 0, fmt::fg(fmt::terminal_color::white), "", format, false, ##__VA_ARGS__)

#define log_msg_ln(format, ...) _log(nullptr, 0, fmt::fg(fmt::terminal_color::white), "", format, true, ##__VA_ARGS__)

#define log_warn(format, ...)                                                                               \
	_log(nullptr, 0, fmt::emphasis::bold | fmt::fg(fmt::terminal_color::yellow), "Warning: ", format, true, \
	     ##__VA_ARGS__)

#define log_err(format, ...)                                                                                   \
	_log(__FILE__, __LINE__, fmt::emphasis::bold | fmt::fg(fmt::terminal_color::red), "Error: ", format, true, \
	     ##__VA_ARGS__)

#define assert_msg(expr, format, ...)   \
	if (!(expr))                        \
	{                                   \
		log_err(format, ##__VA_ARGS__); \
		assert(expr);                   \
	}