#include "script_console.h"


extern "C" void Enik::Console::Trace(const std::string& msg) {
	EN_SCRIPT_MODULE_TRACE(msg);
}

extern "C" void Enik::Console::Trace(const std::string& msg, void* ptr) {
	EN_SCRIPT_MODULE_TRACE(msg, ptr);
}

extern "C" void Enik::Console::Trace(const std::string& msg, const std::string& txt) {
	EN_SCRIPT_MODULE_TRACE(msg, txt);
}

extern "C" void Enik::Console::Trace(const std::string& msg, float x) {
	EN_SCRIPT_MODULE_TRACE(msg, x);
}

extern "C" void Enik::Console::Trace(const std::string& msg, float x, float y) {
	EN_SCRIPT_MODULE_TRACE(msg, x, y);
}

extern "C" void Enik::Console::Trace(const std::string& msg, float x, float y, float z) {
	EN_SCRIPT_MODULE_TRACE(msg, x, y, z);
}



extern "C" void Enik::Console::Info(const std::string& msg) {
	EN_SCRIPT_MODULE_INFO(msg);
}

extern "C" void Enik::Console::Info(const std::string& msg, const std::string& txt) {
	EN_SCRIPT_MODULE_INFO(msg, txt);
}



extern "C" void Enik::Console::Warn(const std::string& msg) {
	EN_SCRIPT_MODULE_WARN(msg);
}

extern "C" void Enik::Console::Warn(const std::string& msg, const std::string& txt) {
	EN_SCRIPT_MODULE_WARN(msg, txt);
}



extern "C" void Enik::Console::Error(const std::string& msg) {
	EN_SCRIPT_MODULE_ERROR(msg);
}

extern "C" void Enik::Console::Error(const std::string& msg, const std::string& txt) {
	EN_SCRIPT_MODULE_ERROR(msg, txt);
}
