#pragma once

#include <glad.h>
#include <vector>
#include <string>

namespace Rocket {
	namespace Utils {
		class APIUtils {
		public:
			static unsigned int loadCubemap(const std::vector<std::string>& faces);
		};

	} // namespace Utils
} // namespace Rocket