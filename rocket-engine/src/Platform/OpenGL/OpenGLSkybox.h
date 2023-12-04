#pragma once

#include "Rocket/Renderer/Skybox.h"

namespace Rocket {

	class OpenGLSkybox : public Skybox {
	public:
		OpenGLSkybox(const std::vector<std::string>& faces);
		virtual ~OpenGLSkybox();

		virtual void resetSkybox(const std::vector<std::string>& faces) override;
		virtual void drawSkybox() override;
		virtual unsigned int getTexture() override;

	private:
		unsigned int m_vao = 0, m_vbo = 0, m_texture = 0;
		std::string m_folderPath;
		std::vector<std::string> m_faces;
	};

} // namespace Rocket