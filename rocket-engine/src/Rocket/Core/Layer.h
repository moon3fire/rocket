#pragma once

#include "Rocket/Core/Core.h"
#include "Rocket/Events/Event.h"
#include "Rocket/Core/Timestep.h"

namespace Rocket {

	class ROCKET_API Layer {

	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep ts) {}
		virtual void onImGuiRender() {}
		virtual void onEvent(Event& event) {}

		inline const std::string& getName() const { return m_debugName; }

	private:
		std::string m_debugName;
	};

} // namespace Rocket

