#pragma once

#include <memory>

#ifdef RCKT_PLATFORM_WINDOWS
	#if RCKT_DYNAMIC_LINK
		#ifdef RCKT_BUILD_DLL
			#define  __declspec(dllexport)
		#else
			#define  __declspec(dllimport)
		#endif
	#else 
		#define RCKT_ENABLE_ASSERTS
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

#define RCKT_BIND_EVENT_FUNC(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

namespace Rocket {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

} // namespace Rocket