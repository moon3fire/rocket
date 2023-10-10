#pragma once

#include "Core.h"

//libs
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Rocket {
	
	class ROCKET_API Log {
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_clientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};


} // namespace Rocket


// Core log macros
#define RCKT_CORE_ERROR(...) ::Rocket::Log::getCoreLogger()->error(__VA_ARGS__)
#define RCKT_CORE_WARN(...)  ::Rocket::Log::getCoreLogger()->warn(__VA_ARGS__)
#define RCKT_CORE_INFO(...)  ::Rocket::Log::getCoreLogger()->info(__VA_ARGS__)
#define RCKT_CORE_TRACE(...) ::Rocket::Log::getCoreLogger()->trace(__VA_ARGS__)

// Client log macros
#define RCKT_ERROR(...)		 ::Rocket::Log::getClientLogger()->error(__VA_ARGS__)
#define RCKT_WARN(...)		 ::Rocket::Log::getClientLogger()->warn(__VA_ARGS__)
#define RCKT_INFO(...)		 ::Rocket::Log::getClientLogger()->info(__VA_ARGS__)
#define RCKT_TRACE(...)		 ::Rocket::Log::getClientLogger()->trace(__VA_ARGS__)