#include "rcktpch.h"
#include "EditorLayer.h"

#include <Rocket/Core/EntryPoint.h>

#include "Rocket/Scene/SceneSerializer.h"
#include "Rocket/Utils/PlatformUtils.h"

#include "ImGuizmo/ImGuizmo.h"

#include "Rocket/Math/Math.h"

namespace Rocket {

	EditorLayer::EditorLayer() :Layer("EditorLayer"), m_cameraController(1280.0f / 720.0f, true) {}

	void EditorLayer::onAttach() {
		RCKT_PROFILE_FUNCTION();

		Rocket::FramebufferSpecification frameBufferSpec;
		frameBufferSpec.width = 1280;
		frameBufferSpec.height = 720;
		m_framebuffer = Rocket::Framebuffer::create(frameBufferSpec);

		m_cameraController.setZoomLevel(5.5f);
		Renderer::setCameraController(m_cameraController);

		m_activeScene = createRef<Scene>();

		/*
		auto square = m_activeScene->createEntity("MYSQUARE");
		square.addComponent<SpriteRendererComponent>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		m_squareEntity = square;

		auto square2 = m_activeScene->createEntity("MYSQUARE2");
		square2.addComponent<SpriteRendererComponent>(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		m_squareEntity2 = square2;

		m_cameraEntity1 = m_activeScene->createEntity("Main camera");
		auto& mainCam = m_cameraEntity1.addComponent<CameraComponent>();
		mainCam.primary = true;

		m_cameraEntity2 = m_activeScene->createEntity("Second camera");
		// already has m_cameraEntity2.addComponent<TagComponent>("THIS IS A TAG");
		auto& second = m_cameraEntity2.addComponent<CameraComponent>();
		second.primary = false;
		*/

		//m_cameraEntity2.addComponent<NativeScriptComponent>().bind<CameraController>();
		//m_cameraEntity1.addComponent<NativeScriptComponent>().bind<CameraController>();

		m_hierarchyPanel.setContext(m_activeScene);
	}

	void EditorLayer::onDetach() {
		RCKT_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Rocket::Timestep ts) {
		RCKT_PROFILE_FUNCTION();

		//resize framebuffer
		resizeFramebuffer();

		//update
		if (m_viewportFocused)
			m_cameraController.onUpdate(ts);

		//render
		Renderer2D::resetStats();

		{
			RCKT_PROFILE_SCOPE("Clear color set:");
			m_framebuffer->bind();
			RenderCommand::setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
			RenderCommand::clear();
		}
		
		{
			RCKT_PROFILE_SCOPE("Scene render");
			//updating scene
			m_activeScene->onUpdate(ts);
		}
		
		m_framebuffer->unbind();
		
	}

	void EditorLayer::onImGuiRender() {
		RCKT_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWindowSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWindowSizeX;
		
		// MENU BAR
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) Rocket::Application::get().close();

				if (ImGui::MenuItem("New", "Ctrl+N")) {
					createNewScene();
				}

				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					openScene();
				}

				if (ImGui::MenuItem("Save as", "Ctrl+Shift+S")) {
					saveSceneAs();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		
		// Settings panel (stats)
		{
			ImGui::Begin("Settings");
			auto stats = Renderer2D::getStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.drawCalls);
			ImGui::Text("Quads: %d", stats.quadCount);
			ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
			ImGui::Text("Indices: %d", stats.getTotalIndexCount());

			ImGui::End();
		}
		{
			m_hierarchyPanel.onImGuiRender();
		}
		ImGui::End();

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			m_viewportFocused = ImGui::IsWindowFocused();
			m_viewportHovered = ImGui::IsWindowHovered();
			Application::get().getImGuiLayer()->blockEvents(!m_viewportFocused && !m_viewportHovered);
		

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		
			m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t textureID = m_framebuffer->getColorAttachmentRendererID();
			ImGui::Image((void*)textureID, ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			// Gizmos
			Entity selectedEntity = m_hierarchyPanel.getSelectedEntity();
			if (selectedEntity && m_gizmosType != -1) {
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				//camera
				auto cameraEntity = m_activeScene->getPrimaryCameraEntity();
				const auto& camera = cameraEntity.getComponent<CameraComponent>().camera;
				const glm::mat4& cameraProjection = camera.getProjection();
				glm::mat4 cameraView = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());

				//entity transform
				auto& transformComponent = selectedEntity.getComponent<TransformComponent>();
				glm::mat4 transform = transformComponent.getTransform();

				// snapping
				bool snap = Input::isKeyPressed(RCKT_KEY_LEFT_SHIFT);
				float snapValue = 0.5f;
				if (m_gizmosType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 10.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_gizmosType,
									 ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing()) {
					glm::vec3 position, scale, rotation;
					Math::DecomposeTransform(transform, position, scale, rotation);

					glm::vec3 deltaRotation = rotation - transformComponent.rotation;
					 
					transformComponent.position = position;
					transformComponent.scale = scale;
					transformComponent.rotation += deltaRotation;
				}
			}

			ImGui::End();
			ImGui::PopStyleVar();
		}

	}

	void EditorLayer::onEvent(Rocket::Event& event) {
		m_cameraController.onEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<KeyPressedEvent>(RCKT_BIND_EVENT_FUNC(EditorLayer::onKeyPressed));
	}

	bool EditorLayer::onKeyPressed(KeyPressedEvent& event)
	{
		//Shortcuts handling
		if (event.getRepeatCount() > 0)
			return false;
			
		bool isControlPressed = Input::isKeyPressed(RCKT_KEY_LEFT_CONTROL) || Input::isKeyPressed(RCKT_KEY_RIGHT_CONTROL);
		bool isShiftPressed = Input::isKeyPressed(RCKT_KEY_LEFT_SHIFT) || Input::isKeyPressed(RCKT_KEY_RIGHT_SHIFT);

		switch (event.getKeyCode()) {
			
			case RCKT_KEY_N: {
				if (isControlPressed)
					createNewScene();
				break;
			}

			case RCKT_KEY_O: {
				if (isControlPressed)
					openScene();
				break;
			}

			case RCKT_KEY_S: {
				if (isControlPressed && isShiftPressed)
					saveSceneAs();
				break;
			}

			// gizmos
			case RCKT_KEY_Q: {
				m_gizmosType++;
				if (m_gizmosType > 2) m_gizmosType = 0;
			}

		}

		return true;
	}

	void EditorLayer::createNewScene() {
		m_activeScene = createRef<Scene>();
		m_activeScene->onViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		m_hierarchyPanel.setContext(m_activeScene);
	}

	void EditorLayer::openScene() {
		std::string filepath = FileDialogs::openFile("Rocket Scene (*.rkct)\0*.rckt\0");
		if (!filepath.empty()) {
			m_activeScene = createRef<Scene>();
			m_activeScene->onViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
			m_hierarchyPanel.setContext(m_activeScene);

			SceneSerializer serializer(m_activeScene);
			serializer.deserialize(filepath);
		}
	}

	void EditorLayer::saveSceneAs() {
		std::string filepath = FileDialogs::saveFile("Rocket Scene (*.rkct)\0*.rckt\0");
		if (!filepath.empty()) {
			SceneSerializer serializer(m_activeScene);
			serializer.serialize(filepath + ".rckt");
		}
	}


	void EditorLayer::resizeFramebuffer() {
		m_specification = m_framebuffer->getSpecification();
		if (m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
			(m_specification.width != m_viewportSize.x || m_specification.height != m_viewportSize.y)) {
			m_framebuffer->resize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
			m_cameraController.onResize(m_viewportSize.x, m_viewportSize.y);

			m_activeScene->onViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		}
	}


} // namespace Rocket