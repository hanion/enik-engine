#pragma once

#ifdef ENIK_ENGINE_BUILD
	#ifdef EN_PLATFORM_WINDOWS
		#define EN_API __declspec(dllexport)
	#elif __GNUC__ >= 4
		#define EN_API __attribute__((visibility("default")))
	#else
		#define EN_API
	#endif
#else
	#ifdef EN_PLATFORM_WINDOWS
		#define EN_API __declspec(dllimport)
	#else
		#define EN_API
	#endif
#endif
