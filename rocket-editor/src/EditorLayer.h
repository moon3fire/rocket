#pragma once

#include "Rocket.h"

#include "panels/SceneHierarchyPanel.h"
#include "panels/ContentBrowserPanel.h"
#include "Rocket/Renderer/EditorCamera.h"

namespace Rocket {

	class EditorLayer : public Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;
		void onEvent(Event& event) override;

		void resizeFramebuffer();

		void onScenePlay(); // being controlled by UI_Toolbar function
		void onSceneStop(); // being controlled by UI_Toolbar function
		//UI
		void UI_Toolbar();

	private:
		bool onKeyPressed(KeyPressedEvent& event);
		bool onMouseButtonPressed(MouseButtonPressedEvent& event);

		void createNewScene();
		void openScene();
		void openScene(const std::filesystem::path& filepath);
		void saveSceneAs();
	
	private:
		//panels
		SceneHierarchyPanel m_hierarchyPanel;
		ContentBrowserPanel m_contentBrowserPanel;

		OrthographicCameraController m_cameraController; //TODO: delete or refactor

		//scene
		Ref<Scene> m_activeScene;
		Ref<Scene> m_editorScene, m_runtimeScene;

		bool m_primaryCamera = true;
		bool m_isUsingFilesystem = false;

		EditorCamera m_editorCamera;

		Ref<Framebuffer> m_framebuffer;
		FramebufferSpecification m_specification;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };
		glm::vec2 m_viewportBounds[2];
		bool m_viewportFocused = false, m_viewportHovered = false;
		bool m_isSkyboxEnabled = false, m_isReflectionEnabled = false, m_isRefractionEnabled = false;

		Entity m_hoveredEntity; // this is related to gizmos functionality so I put it here
		int m_gizmosType = -1; // TODO: make enum from this

		// Scene management UI controll stuff
		enum class SceneState {
			Edit = 0, Play = 1, Paused = 2, Simulate = 3
		};
		SceneState m_sceneState = SceneState::Edit;
		Ref<Texture2D> m_playIcon, m_stopIcon;

	};

} // namespace Rocket