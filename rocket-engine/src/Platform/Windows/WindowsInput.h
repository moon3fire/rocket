#pragma once

#include "Rocket/Core/Input.h"

namespace Rocket {

	class ROCKET_API WindowsInput : public Input {

	protected:
		virtual bool isKeyPressedImpl(int keycode) override;
		virtual bool isMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> getMousePositionsImpl() override;
	};

}