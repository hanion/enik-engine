#pragma once

#include <memory>
#include <asserter.h>

#define EN_PLATFORM_LINUX
#define EN_DEBUG

#ifdef EN_DEBUG
	#if defined(EN_PLATFORM_WINDOWS)
		#define EN_DEBUGBREAK() __debugbreak()
	#elif defined(EN_PLATFORM_LINUX)
		#include <signal.h>
		#define EN_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define EN_ENABLE_ASSERTS
#else
	#define EN_DEBUGBREAK()
#endif


#define EN_EXPAND_MACRO(x) x
#define EN_STRINGIFY_MACRO(x) #x

