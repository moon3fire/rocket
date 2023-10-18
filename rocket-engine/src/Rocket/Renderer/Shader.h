#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Rocket {

	class Shader {
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static Shader* create(const std::string& vertexSource, const std::string& fragmentSource);
	};

} // namespace Rocket