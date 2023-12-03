#pragma once

#define SCENE_MAX_DIRECTIONAL_LIGHTS_COUNT 10
#define SCENE_MAX_POINT_LIGHTS_COUNT 100
#define SCENE_MAX_SPOT_LIGHTS_COUNT 100

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
		Entity createDirectionalLight();
		Entity createPointLight();
		Entity createSpotLight();
		void destroyEntity(Entity entity);

		//TODO: debug purposes, remove
		void clear();
		void debugAllAvailableEntities();

		void onViewportResize(uint32_t width, uint32_t height);
		void onUpdateRuntime(Timestep ts);
		void onUpdateEditor(Timestep ts, EditorCamera& camera, const glm::vec2& viewportSize);

		Entity getPrimaryCameraEntity();
	private:
		template <typename T>
		void onComponentAdded(Entity entity, T& component);
	
	private: // TODO: Move this into editor camera
		static void addCameraController(Entity& entity);
	private:
		entt::registry m_registry;

		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;
		
		int m_entityCount = 0;
		int m_directionalLightCount = 0, m_pointLightCount = 0, m_spotLightCount = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

} // namespace Rocket