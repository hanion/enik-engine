#pragma once

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "core/asserter.h"

namespace Enik {

class Log {

public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
}

#ifdef EN_DEBUG
// Core log macros 
#define EN_CORE_TRACE(...)  ::Enik::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define EN_CORE_INFO(...)   ::Enik::Log::GetCoreLogger()->info(__VA_ARGS__)
#define EN_CORE_WARN(...)   ::Enik::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define EN_CORE_ERROR(...)  ::Enik::Log::GetCoreLogger()->error(__VA_ARGS__)
#define EN_CORE_FATAL(...)  ::Enik::Log::GetCoreLogger()->fatal(__VA_ARGS__)
// Client log macros 
#define EN_TRACE(...)  ::Enik::Log::GetClientLogger()->trace(__VA_ARGS__)
#define EN_INFO(...)   ::Enik::Log::GetClientLogger()->info(__VA_ARGS__)
#define EN_WARN(...)   ::Enik::Log::GetClientLogger()->warn(__VA_ARGS__)
#define EN_ERROR(...)  ::Enik::Log::GetClientLogger()->error(__VA_ARGS__)
#define EN_FATAL(...)  ::Enik::Log::GetClientLogger()->fatal(__VA_ARGS__)
#else
// Core log macros 
#define EN_CORE_TRACE(...)
#define EN_CORE_INFO(...)
#define EN_CORE_WARN(...)
#define EN_CORE_ERROR(...)
#define EN_CORE_FATAL(...)
// Client log macros 
#define EN_TRACE(...)
#define EN_INFO(...)
#define EN_WARN(...)
#define EN_ERROR(...)
#define EN_FATAL(...)
#endif