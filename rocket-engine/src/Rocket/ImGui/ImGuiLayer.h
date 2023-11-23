#pragma once

#include "Rocket/Core/Layer.h"
#include "Rocket/Core/Core.h"

#include "Rocket/Events/KeyEvent.h"
#include "Rocket/Events/MouseEvent.h"
#include "Rocket/Events/ApplicationEvent.h"

namespace Rocket {

	class  ImGuiLayer : public Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onEvent(Event& event) override;
		virtual void onImGuiRender() override;

		void begin();
		void end();

		void blockEvents(bool block) { m_blockEvents = block; }
		void setDarkThemeColors();
	private:
		//variables
		bool m_blockEvents = true;
		float m_time = 0.0f;
	};


} // namespace Rocket