#include "rcktpch.h"
#include "OpenGLRendererAPI.h"

#include <glad.h>

namespace Rocket {
	
	void OpenGLRendererAPI::init() {
		RCKT_PROFILE_FUNCTION();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::setLineThickness(float thickness) {
		glLineWidth(thickness);
	}

	void OpenGLRendererAPI::drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount) {
		vertexArray->bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) {
		vertexArray->bind();
		uint32_t count = indexCount ? vertexArray->getIndexBuffer()->getCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

} // namespace Rocket