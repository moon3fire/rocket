#pragma once

#include "Rocket/Core/Window.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include "GLFW/glfw3.h"

namespace Rocket {
	class WindowsWindow : public Window {
	
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		inline unsigned int getWidth() const override { return m_data.width; }
		inline unsigned int getHeight() const override { return m_data.height; }

		// window attributes
		inline void setEventCallback(const eventCallBackFunc& callback) override { m_data.eventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		inline virtual void* getNativeWindow() const override { return m_window; }

	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();

		GLFWwindow* m_window;
		Scope<GraphicsContext> m_context;

		struct WindowData {
			std::string title;
			unsigned int width, height;
			bool VSync;

			eventCallBackFunc eventCallback;
		};

		WindowData m_data;
	};
} // namespace Rocket

