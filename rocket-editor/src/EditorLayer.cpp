#include "rcktpch.h"
#include "EditorLayer.h"

#include <Rocket/Core/EntryPoint.h>

#include "Rocket/Scene/SceneSerializer.h"
#include "Rocket/Utils/PlatformUtils.h"

#include "ImGuizmo/ImGuizmo.h"

#include "Rocket/Math/Math.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Rocket {

	extern const std::filesystem::path g_assetsDir;

	EditorLayer::EditorLayer() :Layer("EditorLayer"), m_cameraController(1280.0f / 720.0f, true) {}

	void EditorLayer::onAttach() {
		RCKT_PROFILE_FUNCTION();

		FramebufferSpecification frameBufferSpec;
		frameBufferSpec.attachments = { FramebufferTextureFormat::RGBA_16F, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::RGBA_16F, FramebufferTextureFormat::Depth };
		frameBufferSpec.width = 1280;
		frameBufferSpec.height = 720;
		frameBufferSpec.samples = 1;
		m_framebuffer = Framebuffer::create(frameBufferSpec);
		m_specification = frameBufferSpec;

		FramebufferSpecification postProcessingSpec;
		FramebufferSpecification PPFinalSpec;
		postProcessingSpec.attachments = { FramebufferTextureFormat::RGBA_16F };
		postProcessingSpec.width = 1280;
		postProcessingSpec.height = 720;
		postProcessingSpec.samples = 1;

		PPFinalSpec.attachments = { FramebufferTextureFormat::RGBA_16F };
		PPFinalSpec.width = 1280;
		PPFinalSpec.height = 720;
		PPFinalSpec.samples = 1;

		m_pingPongFBO[0] = Framebuffer::create(postProcessingSpec);
		m_pingPongFBO[1] = Framebuffer::create(postProcessingSpec);
		m_postProcessingFramebuffer = Framebuffer::create(PPFinalSpec);

		m_cameraController.setZoomLevel(5.5f);
		Renderer::setCameraController(m_cameraController);

		m_editorCamera = EditorCamera(30.0f, 1778.0f, 0.1f, 1000.0f);

		m_activeScene = createRef<Scene>();
		m_editorScene = m_activeScene;
		// UI TOOLBAR
		m_playIcon = Texture2D::create("resources/icons/editor/play.png");
		m_stopIcon = Texture2D::create("resources/icons/editor/stop.png");

		//m_cameraEntity2.addComponent<NativeScriptComponent>().bind<CameraController>();
		//m_cameraEntity1.addComponent<NativeScriptComponent>().bind<CameraController>();

		m_hierarchyPanel.setContext(m_activeScene);
	}

	void EditorLayer::onDetach() {
		RCKT_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Rocket::Timestep ts) {
		RCKT_PROFILE_FUNCTION();

		//resize framebuffer if needed
		resizeFramebuffer();

		//update
		if (m_viewportFocused) {
			//m_cameraController.onUpdate(ts); old version, TODO: delete
			m_editorCamera.onUpdate(ts);
		}

		//render
		Renderer2D::resetStats();
		{
			RCKT_PROFILE_SCOPE("Clear color set:");
			//m_postProcessingFramebuffer->bind();
			m_framebuffer->bind();
			RenderCommand::setClearColor({ 0.15f, 0.13f, 0.15f, 1.0f });
			RenderCommand::clear();

			// clear entity ID attachment to -1's
			m_framebuffer->clearAttachment(1, -1);
		}

		{
			RCKT_PROFILE_SCOPE("Scene render");
			//updating scene

			switch (m_sceneState) {
			case SceneState::Edit: {
				m_activeScene->onUpdateEditor(ts, m_editorCamera, m_viewportSize);
				break;
			}
			case SceneState::Play: {
				m_activeScene->onUpdateRuntime(ts);
				break;
			}
			}


			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_viewportBounds[0].x;
			my -= m_viewportBounds[0].y;
			glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
			my = viewportSize.y - my;
			int mouseX = (int)mx;
			int mouseY = (int)my;

			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
				int pixelData = m_framebuffer->readPixel(1, mouseX, mouseY);
				m_hoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_activeScene.get());
			}
		}
		//m_postProcessingFramebuffer->unbind();
		m_framebuffer->unbind();

		if (m_isPostprocessingEnabled)
			m_finalTexture = Renderer2D::applyBloom(m_framebuffer, m_pingPongFBO, m_postProcessingFramebuffer, m_editorCamera.getViewProjection());
		else
			m_finalTexture = m_framebuffer->getColorAttachmentRendererID(0);
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
					m_isUsingFilesystem = true;
					createNewScene();
				}

				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					m_isUsingFilesystem = true;
					openScene();
				}

				if (ImGui::MenuItem("Save as", "Ctrl+Shift+S")) {
					m_isUsingFilesystem = true;
					saveSceneAs();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// Settings panel (stats)
		{
			ImGui::Begin("Settings");

			//ImGui::DragFloat3("Diffuse light position", glm::value_ptr(m_diffusePos), 1.0f, 0, 0, "%.2f");
			//ImGui::ColorEdit3("Diffuse light color", glm::value_ptr(m_diffuseColor));
			/*
			if (m_hoveredEntity && m_sceneState == SceneState::Edit) { // TODO:: think about removing file system
				std::string name = "None";
				if (m_hoveredEntity.hasComponent<TagComponent>()) {
					name = m_hoveredEntity.getComponent<TagComponent>().tag;
					ImGui::Text("Hovered entity: %s", name.c_str());
				}
			}
			*/

			// to see all available entities m_activeScene->debugAllAvailableEntities();

			auto stats = Renderer2D::getStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.drawCalls);
			ImGui::Text("Quads: %d", stats.quadCount);
			ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
			ImGui::Text("Indices: %d", stats.getTotalIndexCount());

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


			if (ImGui::Checkbox("Enable skybox", &m_isSkyboxEnabled))
				m_activeScene->enableSkybox(m_isSkyboxEnabled);

			ImGui::Checkbox("Enable HDR", &m_isHDREnabled);
			m_hierarchyPanel.enableHDR(m_isHDREnabled);

			if (m_isHDREnabled) {
				ImGui::Checkbox("Enable postprocessing", &m_isPostprocessingEnabled);
				m_activeScene->enablePostProcessing(m_isPostprocessingEnabled);
			}

			if (m_isSkyboxEnabled) {
				if (ImGui::Button("Change skybox"))
					m_activeScene->changeSkybox();
			}

			if (m_isSkyboxEnabled) {
				if (!m_isRefractionEnabled) {
					if (ImGui::Checkbox("Enable reflection", &m_isReflectionEnabled)) {
						m_activeScene->enableReflection(m_isReflectionEnabled);
					}
				}
				if (!m_isReflectionEnabled) {
					if (ImGui::Checkbox("Enable refraction", &m_isRefractionEnabled)) {
						m_activeScene->enableRefraction(m_isRefractionEnabled);
					}
				}
			}

			if (ImGui::Checkbox("Show Colliders", &m_showColliders))
				m_activeScene->enableColliders(m_showColliders);

			ImGui::End();

		}

		{
			m_hierarchyPanel.onImGuiRender();
			m_contentBrowserPanel.onImGuiRender();
		}

		// UI
		{
			UI_Toolbar();
		}
		ImGui::End();

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");
			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			m_viewportFocused = ImGui::IsWindowFocused();
			m_viewportHovered = ImGui::IsWindowHovered();
			Application::get().getImGuiLayer()->blockEvents(!m_viewportFocused && !m_viewportHovered);

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

			m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			uint32_t textureID = m_finalTexture;
			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });


			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE")) {
					const wchar_t* path = (const wchar_t*)payload->Data;
					openScene(std::filesystem::path(g_assetsDir) / path);
				}
				ImGui::EndDragDropTarget();
			}

			// Gizmos


			Entity selectedEntity = m_hierarchyPanel.getSelectedEntity();
			glm::mat4 projection = m_editorCamera.getProjection();
			glm::mat4 view = m_editorCamera.getViewMatrix();

			if (!Input::isKeyPressed(RCKT_KEY_LEFT_ALT)) {
				if (selectedEntity && m_gizmosType != -1) {
					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();
					ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y, m_viewportBounds[1].x - m_viewportBounds[0].x, m_viewportBounds[1].y - m_viewportBounds[0].y);

					if (m_sceneState == SceneState::Play) {
						auto cameraEntity = m_activeScene->getPrimaryCameraEntity();
						const auto& camera = cameraEntity.getComponent<CameraComponent>().camera;

						RCKT_CORE_ASSERT(cameraEntity, "Trying to use gizmo in non camera running scene!");

						if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic)
							ImGuizmo::SetOrthographic(true);
						projection = camera.getProjection();
						view = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());
					}

					//entity transform
					auto& transformComponent = selectedEntity.getComponent<TransformComponent>();
					glm::mat4 transform = transformComponent.getTransform();

					// snapping
					bool snap = Input::isKeyPressed(RCKT_KEY_LEFT_SHIFT);
					float snapValue = 0.5f;
					if (m_gizmosType == ImGuizmo::OPERATION::ROTATE)
						snapValue = 10.0f;

					float snapValues[3] = { snapValue, snapValue, snapValue };

					ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)m_gizmosType,
						ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

					if (ImGuizmo::IsUsing()) {
						glm::vec3 position = { 0.0f, 0.0f, 0.0f }, scale = { 0.0f, 0.0f, 0.0f }, rotation = { 0.0f, 0.0f, 0.0f };

						Math::DecomposeTransform(transform, position, scale, rotation); // rotation x-z isn't correct

						glm::vec3 deltaRotation = rotation - transformComponent.rotation;
						transformComponent.position = position;
						transformComponent.scale = scale;
						transformComponent.rotation += deltaRotation;
					}
				}
			}
			ImGui::End();
			ImGui::PopStyleVar();
		}

	}

	void EditorLayer::onEvent(Rocket::Event& event) {
		m_cameraController.onEvent(event);
		m_editorCamera.onEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<KeyPressedEvent>(RCKT_BIND_EVENT_FUNC(EditorLayer::onKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(RCKT_BIND_EVENT_FUNC(EditorLayer::onMouseButtonPressed));
	}

	void EditorLayer::UI_Toolbar() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.305f, 0.31f, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() * 0.5f;
		Ref<Texture2D> icon = m_sceneState == SceneState::Edit ? m_playIcon : m_stopIcon;
		ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->getRendererID(), ImVec2{ size + 6.5f, size + 7.0f })) {
			if (m_sceneState == SceneState::Edit)
				onScenePlay();
			else if (m_sceneState == SceneState::Play)
				onSceneStop();
			// TODO: pause, simulate
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::onScenePlay() {
		m_isUsingFilesystem = true;
		m_sceneState = SceneState::Play;

		m_activeScene = Scene::Copy(m_editorScene);
		m_activeScene->onRuntimeStart();

		m_isUsingFilesystem = false;
		m_hierarchyPanel.setContext(m_activeScene);
	}

	void EditorLayer::onSceneStop() {
		m_isUsingFilesystem = true;
		m_activeScene->onRuntimeStop();
		m_activeScene = m_editorScene;

		m_isUsingFilesystem = false;
		m_hierarchyPanel.setSelectedEntity();
		m_hierarchyPanel.setContext(m_activeScene);
		m_sceneState = SceneState::Edit;
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

			 // hierarchy
			case RCKT_KEY_D: {
				if (isControlPressed) {
					if (m_hierarchyPanel.getSelectedEntity()) {
						Entity entity = { m_hierarchyPanel.getSelectedEntity(), m_activeScene.get() };
						m_activeScene->duplicateEntity(entity);
						m_hierarchyPanel.setSelectedEntity();
					}
				}
			
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

	bool EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent& event) {
		if (event.getMouseButton() == RCKT_MOUSE_BUTTON_LEFT) {
			if (m_viewportHovered && !ImGuizmo::IsOver() && !Input::isKeyPressed(RCKT_KEY_LEFT_ALT)) {
				m_hierarchyPanel.setSelectedEntity(m_hoveredEntity);
			}
		}
		return false;
	}

	void EditorLayer::createNewScene() {

		m_activeScene = createRef<Scene>();
		m_activeScene->onViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		m_hierarchyPanel.setContext(m_activeScene);
	}

	void EditorLayer::openScene() {
		std::string filepath = FileDialogs::openFile("Rocket Scene (*.rkct)\0*.rckt\0");
		if (!filepath.empty())
			openScene(filepath);
	}

	// this one is used for drag&dropping
	void EditorLayer::openScene(const std::filesystem::path& filepath) {
		m_activeScene->clear();
		if (m_sceneState != SceneState::Edit)
			onSceneStop();

		if (filepath.extension().string() != ".rckt") {
			RCKT_CORE_WARN("Could not load {0} - is not a scene file!", filepath.string());
			return;
		}

		Ref<Scene> newScene = createRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.deserialize(filepath.string())) {
			m_editorScene = newScene;
			m_editorScene->onViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
			m_hierarchyPanel.setContext(m_editorScene);
			m_activeScene = m_editorScene;

		}
	}

	void EditorLayer::saveSceneAs() {
		std::string filepath = FileDialogs::saveFile("Rocket Scene (*.rkct)\0*.rckt\0");
		if (!filepath.empty()) {
			SceneSerializer serializer(m_activeScene);
			serializer.serialize(filepath + ".rckt");
		}
		m_isUsingFilesystem = false;
	}


	void EditorLayer::resizeFramebuffer() {
		if (!m_isPostprocessingEnabled) {
			m_specification = m_framebuffer->getSpecification();
			if (m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
				(m_specification.width != m_viewportSize.x || m_specification.height != m_viewportSize.y)) {
				m_framebuffer->resize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
				m_cameraController.onResize(m_viewportSize.x, m_viewportSize.y);

				m_editorCamera.setViewportSize(m_viewportSize.x, m_viewportSize.y);

				m_activeScene->onViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
			}
		}
		else {
			m_specification = m_postProcessingFramebuffer->getSpecification();
			
			if (m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
				(m_specification.width != m_viewportSize.x || m_specification.height != m_viewportSize.y)) {

				//RCKT_CORE_INFO("Im reaching to here");
				m_postProcessingFramebuffer->resize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
				m_cameraController.onResize(m_viewportSize.x, m_viewportSize.y);

				m_editorCamera.setViewportSize(m_viewportSize.x, m_viewportSize.y);
				m_activeScene->onViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
			}
		}
	}


} // namespace Rocket