#pragma once

#include "Core.h"

//libs
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace Rocket {
	
	class  Log {
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
#define RCKT_CORE_ERROR(...)	::Rocket::Log::getCoreLogger()->error(__VA_ARGS__)
#define RCKT_CORE_WARN(...)		::Rocket::Log::getCoreLogger()->warn(__VA_ARGS__)
#define RCKT_CORE_INFO(...)		::Rocket::Log::getCoreLogger()->info(__VA_ARGS__)
#define RCKT_CORE_TRACE(...)	::Rocket::Log::getCoreLogger()->trace(__VA_ARGS__)
#define RCKT_CORE_CRITICAL(...) ::Rocket::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define RCKT_ERROR(...)			::Rocket::Log::getClientLogger()->error(__VA_ARGS__)
#define RCKT_WARN(...)			::Rocket::Log::getClientLogger()->warn(__VA_ARGS__)
#define RCKT_INFO(...)			::Rocket::Log::getClientLogger()->info(__VA_ARGS__)
#define RCKT_TRACE(...)			::Rocket::Log::getClientLogger()->trace(__VA_ARGS__)
#define RCKT_CRITICAL(...)		::Rocket::Log::getClientLogger()->critical(__VA_ARGS__)