#pragma once

#include "RendererAPI.h"

#include <glm/glm.hpp>

namespace Rocket {

	class RenderCommand {

	public:

		static void init() {
			RCKT_PROFILE_FUNCTION();
			s_rendererAPI->init();
		}

		static void setClearColor(const glm::vec4& color) {
			s_rendererAPI->setClearColor(color);
		}

		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_rendererAPI->setViewport(x, y, width, height);
		}

		static void clear() {
			s_rendererAPI->clear();
		}

		static void setLineThickness(float thickness) {
			s_rendererAPI->setLineThickness(thickness);
		}

		static void drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) {
			s_rendererAPI->drawLines(vertexArray, vertexCount);
		}

		static void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) {
			s_rendererAPI->drawIndexed(vertexArray, indexCount);
		}

	private:
		static Scope<RendererAPI> s_rendererAPI;
	};


} // namespace Rocket