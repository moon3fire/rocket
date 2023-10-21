#pragma once

#include <memory>

#ifdef RCKT_PLATFORM_WINDOWS
	#if RCKT_DYNAMIC_LINK
		#ifdef RCKT_BUILD_DLL
			#define ROCKET_API __declspec(dllexport)
		#else
			#define ROCKET_API __declspec(dllimport)
		#endif
	#else 
		#define ROCKET_API
	#endif
#endif

#ifdef RCKT_ENABLE_ASSERTS
	#define RCKT_ASSERT(x, ...) { if(!(x)) { RCKT_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define RCKT_CORE_ASSERT(x, ...) { if(!(x)) { RCKT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define RCKT_ASSERT(x, ...)
	#define RCKT_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define RCKT_BIND_EVENT_FUNC(func) std::bind(&func, this, std::placeholders::_1) 

namespace Rocket {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

} // namespace Rocket