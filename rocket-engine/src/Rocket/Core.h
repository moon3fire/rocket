#pragma once

#ifdef RCKT_PLATFORM_WINDOWS
	#ifdef RCKT_BUILD_DLL
		#define ROCKET_API __declspec(dllexport)
	#else
		#define ROCKET_API __declspec(dllimport)
	#endif
#else 
	#error Rocket only supports Windows!
#endif

#ifdef RCKT_ENABLE_ASSERTS
	#define RCKT_ASSERT(x, ...) { if(!(x)) { RCKT_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define RCKT_CORE_ASSERT(x, ...) { if(!(x)) { RCKT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define RCKT_ASSERT(x, ...)
	#define RCKT_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)