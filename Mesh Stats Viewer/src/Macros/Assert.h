#pragma once

#if defined(_MSC_VER)
#	define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#	define DEBUG_BREAK() __builtin_trap()
#else
#	include <csignal>
#	define DEBUG_BREAK() raise(SIGTRAP)
#endif

#ifdef DEBUG
#	define ASSERT(expression) do { if (!(expression)) { LOG_ERROR("Assertion failed: \"{}\", File: {}, Line: {}", #expression, __FILE__, __LINE__); DEBUG_BREAK(); } } while(false)
#else
#	define ASSERT(expression) do { if (!(expression)) abort(); } while(false)
#endif