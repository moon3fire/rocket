#include "rcktpch.h"
#include "Entity.h"

namespace Rocket {
	Entity::Entity() :m_entityHandle{ entt::null }, m_scene(nullptr) {}
	Entity::Entity(entt::entity handle, Scene* scene) :m_entityHandle(handle), m_scene(scene) {

	}

} // namespace Rocket