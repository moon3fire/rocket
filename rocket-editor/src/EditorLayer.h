#pragma once

#include "Rocket.h"

#include "panels/SceneHierarchyPanel.h"
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
	private:
		bool onKeyPressed(KeyPressedEvent& event);

		void createNewScene();
		void openScene();
		void saveSceneAs();
	
	private:
		//panels
		SceneHierarchyPanel m_hierarchyPanel;

		OrthographicCameraController m_cameraController;

		//scene
		Ref<Scene> m_activeScene;

		bool m_primaryCamera = true;

		EditorCamera m_editorCamera;

		Ref<Framebuffer> m_framebuffer;
		FramebufferSpecification m_specification;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };
		bool m_viewportFocused = false, m_viewportHovered = false;

		int m_gizmosType = -1;
	};

} // namespace Rocket