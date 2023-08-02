#pragma once

#include <memory>
#include "core/log.h"

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

#define EN_BIND_EVENT_FN(fn) std::bind(&Enik::fn, this, std::placeholders::_1)


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