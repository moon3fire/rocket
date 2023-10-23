#pragma once

#include "Rocket/Core/Core.h"

namespace Rocket {

	class ROCKET_API Input {

	public:
		static inline bool isKeyPressed(int keycode) { return s_instance->isKeyPressedImpl(keycode); }
		static inline bool isMouseButtonPressed(int button) { return s_instance->isMouseButtonPressedImpl(button); }
		static inline std::pair<float, float> getMousePositions() { return s_instance->getMousePositionsImpl(); }
	protected:
		virtual bool isKeyPressedImpl(int keycode) = 0;
		virtual bool isMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> getMousePositionsImpl() = 0;

	private:
		static Scope<Input> s_instance;

	};

} // namespace Rocket