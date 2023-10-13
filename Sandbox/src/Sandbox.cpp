#include "rcktpch.h"
#include <Rocket.h>

#include "../vendors/imgui/imgui.h"

class ExampleLayer : public Rocket::Layer {
public:
	ExampleLayer() :Layer("example") {

	}

	void onUpdate() override {
		//RCKT_INFO("ExampleLayer::Update");
		
	}

	virtual void onImGuiRender() override {
		ImGui::Begin("Test");
		ImGui::Text("hello world");
		ImGui::End();
	}

	void onEvent(Rocket::Event& event) override {
		//RCKT_TRACE("{0}", event);
		if (Rocket::Input::isKeyPressed(RCKT_KEY_0)) {
			RCKT_INFO("0 key is being pressed");
		}
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