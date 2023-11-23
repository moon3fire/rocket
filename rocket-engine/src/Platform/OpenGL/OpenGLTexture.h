#pragma once

#include "Rocket/Renderer/Texture.h"

#include <glad.h>

namespace Rocket {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(int width, int height);
		virtual ~OpenGLTexture2D();

		virtual void setData(void* data, uint32_t size) override;

		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }

		virtual void bind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other) const override { 
			return m_rendererID == ((OpenGLTexture2D&)other).m_rendererID;
		}
	private:

		std::string m_path;
		uint32_t m_width, m_height;
		uint32_t m_rendererID;
		GLenum m_internalFormat, m_dataFormat;
	};

} // namespace Rocket