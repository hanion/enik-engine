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
	inline static std::shared_ptr<spdlog::logger>& GetScriptLogger() { return s_ScriptLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
	static std::shared_ptr<spdlog::logger> s_ScriptLogger;
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
// Script log macros
#define EN_SCRIPT_MODULE_TRACE(...)  ::Enik::Log::GetScriptLogger()->trace(__VA_ARGS__)
#define EN_SCRIPT_MODULE_INFO(...)   ::Enik::Log::GetScriptLogger()->info(__VA_ARGS__)
#define EN_SCRIPT_MODULE_WARN(...)   ::Enik::Log::GetScriptLogger()->warn(__VA_ARGS__)
#define EN_SCRIPT_MODULE_ERROR(...)  ::Enik::Log::GetScriptLogger()->error(__VA_ARGS__)
#define EN_SCRIPT_MODULE_FATAL(...)  ::Enik::Log::GetScriptLogger()->fatal(__VA_ARGS__)
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
// Script log macros
#define EN_SCRIPT_MODULE_TRACE(...)
#define EN_SCRIPT_MODULE_INFO(...)
#define EN_SCRIPT_MODULE_WARN(...)
#define EN_SCRIPT_MODULE_ERROR(...)
#define EN_SCRIPT_MODULE_FATAL(...)
#endif