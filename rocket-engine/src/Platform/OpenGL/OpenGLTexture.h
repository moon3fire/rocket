#pragma once

#include "Rocket/Renderer/Texture.h"

namespace Rocket {

	class OpenGLTexture2D : public Texture2D {
	public:
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }

		virtual void bind(uint32_t slot = 0) const override;

	private:

		std::string m_path;
		uint32_t m_width, m_height;
		uint32_t m_rendererID;

	};

} // namespace Rocket