#pragma once

#include <entt.hpp>

#include "Rocket/Core/Timestep.h"

namespace Rocket {

	class Entity;

	class Scene {
	public:

		Scene();
		~Scene();

		Entity createEntity(const std::string& name = std::string());

		void onUpdate(Timestep ts);
	private:
		entt::registry m_registry;

		friend class Entity;
	};

} // namespace Rocket