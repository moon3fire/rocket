#pragma once

#include "Scene.h"

#include "entt.hpp"

namespace Rocket {

	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& addComponent(Args&&... args) {
			RCKT_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
			return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		T& getComponent() {
			RCKT_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
			return m_scene->m_registry.get<T>(m_entityHandle);
		}

		template <typename T>
		bool hasComponent() {
			return m_scene->m_registry.any_of<T>(m_entityHandle);
		}

		template <typename T>
		void removeComponent() {
			RCKT_CORE_ASSERT(hasComponent<T>(), "Entity doesn't have specified component already!");
			m_scene->m_registry.remove<T>(m_entityHandle);
		}

		operator bool() const { return m_entityHandle != entt::null; }

	private:
		entt::entity m_entityHandle{ entt::null };
		Scene* m_scene = nullptr;
	};

} // namespace Rocket