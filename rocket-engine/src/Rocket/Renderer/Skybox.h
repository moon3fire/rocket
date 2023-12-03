#pragma once

#include "Rocket/Core/Core.h"
#include <vector>
#include <string>

namespace Rocket {

	class Skybox {
	public:
		virtual ~Skybox() = default;

		static Ref<Skybox> create(const std::vector<std::string>& faces);
		virtual void resetSkybox(const std::vector<std::string>& faces) = 0;
		virtual void drawSkybox() = 0;
	};

} // namespace Rocket