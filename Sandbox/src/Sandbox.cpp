#include "rcktpch.h"
#include <Rocket.h>

class ExampleLayer : public Rocket::Layer {
public:
	ExampleLayer() :Layer("example") {

	}

	void onUpdate() override {
		//RCKT_INFO("ExampleLayer::Update");
	}

	void onEvent(Rocket::Event& event) override {
		RCKT_TRACE("{0}", event);
	}
};

class Sandbox : public Rocket::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};

Rocket::Application* Rocket::createApplication() {
	return new Sandbox();
}