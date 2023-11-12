#include "rcktpch.h"
#include "Entity.h"

namespace Rocket {

	Entity::Entity(entt::entity handle, Scene* scene) :m_entityHandle(handle), m_scene(scene) {

	}

} // namespace Rocket