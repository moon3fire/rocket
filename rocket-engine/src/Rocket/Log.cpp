#include "Log.h"

namespace Rocket {
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger>	Log::s_clientLogger;

	void Log::init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_coreLogger = spdlog::stdout_color_mt("Rocket");
		s_coreLogger->set_level(spdlog::level::trace);
		s_clientLogger = spdlog::stdout_color_mt("Sandbox");
		s_clientLogger->set_level(spdlog::level::trace);
	}


} // namespace Rocket