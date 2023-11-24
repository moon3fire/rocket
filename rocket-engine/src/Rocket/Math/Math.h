#pragma once

#include <glm/glm.hpp>

namespace Rocket { namespace Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outScale, glm::vec3& outRotation);



} } // namespace Rocket