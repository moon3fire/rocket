#pragma once

#include "rcktpch.h"

#include "Rocket/Core.h"
#include "Rocket/Events/Event.h"

namespace Rocket {

	struct WindowProps {
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProps(const std::string& title_ = "Rocket engine", unsigned int width_ = 1280,
					unsigned int height_ = 720) :width(width_), height(height_), title(title_) { }
	};

	// interface representing a desktop system based window
	class ROCKET_API Window {

	public:
		using eventCallBackFunc = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void onUpdate() = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		// window attributes
		virtual void setEventCallback(const eventCallBackFunc& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};

} // namespace Rocket