#include "rcktpch.h"
#include "OpenGLVertexArray.h"

#include <glad.h>

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

	OpenGLVertexArray::OpenGLVertexArray() :m_rendererID(0) {
		RCKT_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::bind() const {
		RCKT_PROFILE_FUNCTION();

		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::unbind() const {
		RCKT_PROFILE_FUNCTION();
		
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) {
		RCKT_PROFILE_FUNCTION();

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
				(void*)(intptr_t)elem.offset);

			index++;
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		RCKT_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) {
		RCKT_PROFILE_FUNCTION();

		glBindVertexArray(m_rendererID);
		indexBuffer->bind();
		m_indexBuffer = indexBuffer;
	}

} // namespace Rocket