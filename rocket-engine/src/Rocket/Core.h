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