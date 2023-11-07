#include "rcktpch.h"
#include "Sandbox2D.h"

class Sandbox : public Rocket::Application {
public:
	Sandbox() {
		pushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

Rocket::Application* Rocket::createApplication() {
	return new Sandbox();
}