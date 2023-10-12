#pragma once

#include "Rocket/Core.h"
#include "Rocket/Events/Event.h"

namespace Rocket {

	class ROCKET_API Layer {

	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate() {}
		virtual void onImGuiRender() {}
		virtual void onEvent(Event& event) {}

		inline const std::string& getName() const { return m_debugName; }

	private:
		std::string m_debugName;
	};

} // namespace Rocket

