#pragma once

#include "Rocket/Core/Layer.h"
#include "Rocket/Core/Core.h"

#include "Rocket/Events/KeyEvent.h"
#include "Rocket/Events/MouseEvent.h"
#include "Rocket/Events/ApplicationEvent.h"

namespace Rocket {

	class ROCKET_API ImGuiLayer : public Layer {

	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onImGuiRender() override;

		void begin();
		void end();

	private:
		//variables
		float m_time = 0.0f;
	};


} // namespace Rocket