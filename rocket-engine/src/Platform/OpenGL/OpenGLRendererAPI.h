#pragma once

#include "Rocket/Renderer/RendererAPI.h"

#include <glm/glm.hpp>

namespace Rocket {

	class OpenGLRendererAPI : public RendererAPI {

	public:
		virtual void init() override;

		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;

		virtual void drawIndexed(const Ref<VertexArray>& vertexArray) override;

	private:

	};

} // namespace Rocket