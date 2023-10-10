#pragma once

#ifdef RCKT_PLATFORM_WINDOWS
	extern Rocket::Application* Rocket::createApplication();

	int main(int argc, char** argv)
	{
		auto app = Rocket::createApplication();
		app->run();
		delete app;
	}
#endif 