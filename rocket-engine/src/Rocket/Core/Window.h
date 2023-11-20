#pragma once

#include "rcktpch.h"

#include "Rocket/Core/Core.h"
#include "Rocket/Events/Event.h"

namespace Rocket {

	struct WindowProps {
		std::string title;
		uint32_t width;
		uint32_t height;

		WindowProps(const std::string& title_ = "Rocket engine", uint32_t width_ = 1600,
					uint32_t height_ = 900) :width(width_), height(height_), title(title_) { }
	};

	// interface representing a desktop system based window
	class ROCKET_API Window {

	public:
		using eventCallBackFunc = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void onUpdate() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		// window attributes
		virtual void setEventCallback(const eventCallBackFunc& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};

} // namespace Rocket