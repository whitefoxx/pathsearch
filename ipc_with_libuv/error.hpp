#ifndef IPC_ERROR_HPP
#define IPC_ERROR_HPP

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

class Error : public std::runtime_error
{
public:
	explicit Error(const char *description) : std::runtime_error(description)
	{
	}

public:
	static const size_t bufferSize{2000};
	thread_local static char buffer[];
};

// clang-format off
#define CLASS "ipc-demo"
#define _LOG_STR "%s::%s()"
#define _LOG_STR_DESC _LOG_STR " | "
#define _LOG_ARG CLASS, __FUNCTION__
#define _LOG_SEPARATOR_CHAR_STD "\n"

#define ERROR_STD(desc, ...) \
	do \
	{ \
        std::fprintf(stderr, _LOG_STR_DESC desc _LOG_SEPARATOR_CHAR_STD, _LOG_ARG, ##__VA_ARGS__); \
        std::fflush(stderr); \
	} while (false)

#define THROW_ERROR_STD(desc, ...) \
	do \
	{ \
		ERROR_STD("throwing Error: " desc, ##__VA_ARGS__); \
		std::snprintf(Error::buffer, Error::bufferSize, desc, ##__VA_ARGS__); \
		throw Error(Error::buffer); \
	} while (false)

#define ABORT(desc, ...) \
	do \
	{ \
        std::fprintf(stderr, _LOG_STR_DESC desc _LOG_SEPARATOR_CHAR_STD, _LOG_ARG, ##__VA_ARGS__); \
        std::fflush(stderr); \
		std::abort(); \
	} while (false)

#endif
