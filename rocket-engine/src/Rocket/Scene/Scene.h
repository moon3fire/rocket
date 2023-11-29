#pragma once

#include "Rocket/Renderer/EditorCamera.h"
#include "Rocket/Core/Timestep.h"

#include <entt.hpp>

namespace Rocket {

	class Entity;
	class SceneHierarchyPanel;

	class Scene {
	public:

		Scene();
		~Scene();

		Entity createEntity(const std::string& name = std::string());
		void destroyEntity(Entity entity);

		//TODO: debug purposes, remove
		void debugAllAvailableEntities();

		void onViewportResize(uint32_t width, uint32_t height);
		void onUpdateRuntime(Timestep ts);
		void onUpdateEditor(Timestep ts, EditorCamera& camera, const glm::vec3& diffusePos = glm::vec3(0.0f), const glm::vec3& diffuseColor = glm::vec3(1.0f));

		Entity getPrimaryCameraEntity();
	private:
		template <typename T>
		void onComponentAdded(Entity entity, T& component);
	
	private: // TODO: Move this into editor camera
		static void addCameraController(Entity& entity);
	private:
		entt::registry m_registry;

		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;
		uint32_t m_entityCount = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

} // namespace Rocket