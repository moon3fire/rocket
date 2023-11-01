#pragma once

#ifdef RCKT_PLATFORM_WINDOWS
	extern Rocket::Application* Rocket::createApplication();

	int main(int argc, char** argv)
	{
		Rocket::Log::init();

		RCKT_PROFILE_BEGIN_SESSION("StartUp", "RocketProfile-StartUp.json");
		auto app = Rocket::createApplication();
		RCKT_PROFILE_END_SESSION();

		RCKT_PROFILE_BEGIN_SESSION("Runtime", "RocketProfile-Runtime.json");
		app->run();
		RCKT_PROFILE_END_SESSION();
		
		RCKT_PROFILE_BEGIN_SESSION("Shutdown", "RocketProfile-Shutdown.json");
		delete app;
		RCKT_PROFILE_END_SESSION();
	}
#endif 