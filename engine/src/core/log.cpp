#include "log.h"

namespace Enik {
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
std::shared_ptr<spdlog::logger> Log::s_ScriptLogger;

std::shared_ptr<spdlog::logger>& Log::GetCoreLogger()   { return Log::s_CoreLogger; }
std::shared_ptr<spdlog::logger>& Log::GetClientLogger() { return Log::s_ClientLogger; }
std::shared_ptr<spdlog::logger>& Log::GetScriptLogger() { return Log::s_ScriptLogger; }

void Log::Init() {
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_CoreLogger = spdlog::stdout_color_mt("EN");
	s_CoreLogger->set_level(spdlog::level::trace);

	s_ClientLogger = spdlog::stdout_color_mt("APP");
	s_ClientLogger->set_level(spdlog::level::trace);

	s_ScriptLogger = spdlog::stdout_color_mt("Console");
	s_ScriptLogger->set_level(spdlog::level::trace);

}
}
