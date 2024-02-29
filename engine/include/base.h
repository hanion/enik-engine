#pragma once

#include <memory>
#include "core/log.h"


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

#define EN_BIND_EVENT_FN(fn) std::bind(&Enik::fn, this, std::placeholders::_1)

// path relative to ./build/sandbox 
#define FULL_PATH(x) (((std::string)"../../sandbox/") + ((std::string)x))
#define FULL_PATH_EDITOR(x) (((std::string)"../../editor/") + ((std::string)x))

namespace Enik {

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}


#if defined(EN_DEBUG) && !defined(EN_PLATFORM_WINDOWS)
#include "Tracy.hpp"
#define EN_PROFILE_FRAME(x) FrameMark
#define EN_PROFILE_SCOPE ZoneScoped
#define EN_PROFILE_SECTION(x) ZoneScopedN(x)
#else
#define EN_PROFILE_FRAME(x)
#define EN_PROFILE_SCOPE
#define EN_PROFILE_SECTION(x)
#endif



#ifdef EN_PLATFORM_WINDOWS
	#ifndef and
		#define and &&
	#endif
	#ifndef or
		#define or ||
	#endif
	#ifndef not
		#define not !
	#endif

	#define NOMINMAX
	#ifdef max
		#undef max
	#endif
	#ifdef min
		#undef min
	#endif

	#ifdef near
		#undef near
	#endif
	#ifdef far
		#undef far
	#endif
#endif

