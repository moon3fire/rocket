#include "rcktpch.h"
#include <Rocket.h>


class Sandbox : public Rocket::Application {
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};

Rocket::Application* Rocket::createApplication() {
	return new Sandbox();
}