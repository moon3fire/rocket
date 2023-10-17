#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Rocket {

	class Shader {
	public:
		Shader(const std::string& vertexSource, const std::string& fragmentSource);
		~Shader();

		void bind() const;
		void unbind() const;

		void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		uint32_t m_rendererID;
	};

} // namespace Rocket