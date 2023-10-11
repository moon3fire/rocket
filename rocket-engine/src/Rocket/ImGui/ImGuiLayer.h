#pragma once

#include "Rocket/Layer.h"
#include "Rocket/Core.h"

#include "Rocket/Events/KeyEvent.h"
#include "Rocket/Events/MouseEvent.h"
#include "Rocket/Events/ApplicationEvent.h"

namespace Rocket {

	class ROCKET_API ImGuiLayer : public Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onAttach() override;
		void onDetach() override;
		void onUpdate() override;
		void onEvent(Event& event) override;

	private:

		bool onMouseButtonPressedEvent(MouseButtonPressedEvent& event);
		bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);
		bool onMouseMovedEvent(MouseMovedEvent& event);
		bool onMouseScrolledEvent(MouseScrolledEvent& event);
		bool onKeyPressedEvent(KeyPressedEvent& event);
		bool onKeyReleasedEvent(KeyReleasedEvent& event);
		bool onKeyTypedEvent(KeyTypedEvent& event);
		bool onWindowResizedEvent(WindowResizeEvent& event);

		//variables
		float m_time = 0.0f;
	};


} // namespace Rocket