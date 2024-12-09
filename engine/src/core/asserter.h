#pragma once

#include <base.h>

#include <filesystem>

#define EN_ENABLE_ASSERTS
#define EN_ENABLE_VERIFY


#ifdef EN_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define EN_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
	{                                                  \
		if (!(check)) {                                \
			EN##type##ERROR(msg, __VA_ARGS__);         \
			EN_DEBUGBREAK();                           \
		}                                              \
	}
#define EN_INTERNAL_ASSERT_WITH_MSG(type, check, ...) EN_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define EN_INTERNAL_ASSERT_NO_MSG(type, check) EN_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", EN_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define EN_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define EN_INTERNAL_ASSERT_GET_MACRO(...) EN_EXPAND_MACRO(EN_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, EN_INTERNAL_ASSERT_WITH_MSG, EN_INTERNAL_ASSERT_NO_MSG))

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define EN_ASSERT(...) EN_EXPAND_MACRO(EN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define EN_CORE_ASSERT(...) EN_EXPAND_MACRO(EN_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define EN_ASSERT(...)
#define EN_CORE_ASSERT(...)
#endif







#ifdef EN_ENABLE_VERIFY

#define EN_INTERNAL_VERIFY_IMPL(type, check, msg, ...) \
	{                                                  \
		if (!(check)) {                                \
			EN##type##ERROR(msg, __VA_ARGS__);         \
			EN_DEBUGBREAK();                           \
		}                                              \
	}
#define EN_INTERNAL_VERIFY_WITH_MSG(type, check, ...) EN_INTERNAL_VERIFY_IMPL(type, check, "Verify failed: {0}", __VA_ARGS__)
#define EN_INTERNAL_VERIFY_NO_MSG(type, check) EN_INTERNAL_VERIFY_IMPL(type, check, "Verify '{0}' failed at {1}:{2}", EN_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define EN_INTERNAL_VERIFY_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define EN_INTERNAL_VERIFY_GET_MACRO(...) EN_EXPAND_MACRO(EN_INTERNAL_VERIFY_GET_MACRO_NAME(__VA_ARGS__, EN_INTERNAL_VERIFY_WITH_MSG, EN_INTERNAL_VERIFY_NO_MSG))

#define EN_VERIFY(...) EN_EXPAND_MACRO(EN_INTERNAL_VERIFY_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define EN_CORE_VERIFY(...) EN_EXPAND_MACRO(EN_INTERNAL_VERIFY_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define EN_VERIFY(...)
#define EN_CORE_VERIFY(...)
#endif


