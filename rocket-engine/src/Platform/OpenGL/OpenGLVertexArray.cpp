#include "rcktpch.h"
#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Rocket {

	static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Bool:		return GL_BOOL;
		}

		RCKT_CORE_ASSERT(false, "Unknown shader data type");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::bind() const {
		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::unbind() const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {

		RCKT_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "Vertex buffer has no layout");
		
		glBindVertexArray(m_rendererID);
		vertexBuffer->bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->getLayout();
		for (const auto& elem : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				GetElementCount(elem.type),
				ShaderDataTypeToOpenGLType(elem.type),
				elem.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(void*)elem.offset);

			index++;
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
		glBindVertexArray(m_rendererID);
		indexBuffer->bind();
		m_indexBuffer = indexBuffer;
	}

} // namespace Rocket