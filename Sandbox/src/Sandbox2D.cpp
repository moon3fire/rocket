#include "rcktpch.h"
#include "Sandbox2D.h"

#include <Rocket/Core/EntryPoint.h>

Sandbox2D::Sandbox2D() :Layer("Sandbox2D"), m_cameraController(1280.0f / 720.0f, true) {
	onAttach();
}

void Sandbox2D::onAttach() {
	RCKT_PROFILE_FUNCTION();

	m_squareWoodTexture = Rocket::Texture2D::create("assets/textures/wood.png");
}

void Sandbox2D::onDetach() {
	RCKT_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Rocket::Timestep ts) {	
	RCKT_PROFILE_FUNCTION();

	//update
	
	m_cameraController.onUpdate(ts);
	
	//render
	{
		RCKT_PROFILE_SCOPE("Clear color set:");
		Rocket::RenderCommand::setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		Rocket::RenderCommand::clear();
	}
	{
		RCKT_PROFILE_SCOPE("Scene render");
		Rocket::Renderer2D::beginScene(m_cameraController.getCamera());
		{
			Rocket::Renderer2D::drawQuad2DWithTexture({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });

			Rocket::Renderer2D::drawQuad2DWithTexture({ 1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.2f, 0.2f, 0.9f, 1.0f });
		}
		Rocket::Renderer2D::endScene();
	}
}

void Sandbox2D::onImGuiRender() {
	RCKT_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	if (ImGui::TreeNode("Square")) {
		ImGui::ColorEdit4("Color Square", glm::value_ptr(m_squareColor));
		ImGui::DragFloat2("Position Square", glm::value_ptr(m_squarePosition), 0.002f);
		ImGui::DragFloat2("Scale Square", glm::value_ptr(m_squareScale), 0.002f);
		ImGui::DragFloat("Rotation Square", &m_squareRotation, 0.002f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Background")) {
		ImGui::ColorEdit4("Color##Square2", glm::value_ptr(m_backgroundColor));
		ImGui::DragFloat2("Position##Square2", glm::value_ptr(m_backgroundPosition), 0.002f);
		ImGui::DragFloat2("Scale##Square2", glm::value_ptr(m_backgroundScale), 0.002f);
		ImGui::TreePop();
	}

	ImGui::End();
}

void Sandbox2D::onEvent(Rocket::Event& event) {
	m_cameraController.onEvent(event);
}
