#pragma once

#ifdef DEBUG
#	include <iostream>
#	define LOG_INFO(fmt, ...) do { std::cout << "[INFO] " << std::format(fmt, __VA_ARGS__) << std::endl; } while(false)
#	define LOG_WARNING(fmt, ...) do { std::cout << "[WARNING] " << std::format(fmt, __VA_ARGS__) << std::endl; } while(false)
#	define LOG_ERROR(fmt, ...) do { std::cerr << "[ERROR] " << std::format(fmt, __VA_ARGS__) << std::endl; } while(false)
#else
#	define LOG_INFO(fmt, ...)
#	define LOG_WARNING(fmt, ...)
#	define LOG_ERROR(fmt, ...)
#endif