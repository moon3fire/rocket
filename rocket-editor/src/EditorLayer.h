#pragma once

#include "Rocket.h"

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

		OrthographicCameraController m_cameraController;

		//scene
		Ref<Scene> m_activeScene;
		Entity m_squareEntity;
		Entity m_cameraEntity1;
		Entity m_cameraEntity2;

		bool m_primaryCamera = true;

		Ref<Framebuffer> m_framebuffer;
		FramebufferSpecification m_specification;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };
		bool m_viewportFocused = false, m_viewportHovered = false;
	};

} // namespace Rocket