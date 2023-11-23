#pragma once

#include "Rocket/Core/Core.h"

namespace Rocket {

	class  Input {

	public:
		virtual ~Input() = default;

		static bool isKeyPressed(int keycode);
		static bool isMouseButtonPressed(int button);
		static std::pair<float, float> getMousePosition();
		static float getMouseX();
		static float getMouseY();
	};

} // namespace Rocket