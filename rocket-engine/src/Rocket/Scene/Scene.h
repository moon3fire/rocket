#pragma once

#include "Rocket/Renderer/EditorCamera.h"
#include "Rocket/Core/Timestep.h"
#include "Rocket/Core/UUID.h"

#include <entt.hpp>

namespace Rocket {

	class Entity;
	class SceneHierarchyPanel;

	class Scene {
	public:

		Scene();
		~Scene();

		Entity createEntity(const std::string& name = "", const UUID& uuid = 0);
		Entity createDirectionalLight(const UUID& uuid = 0);
		Entity createPointLight(const UUID& uuid = 0);
		Entity createSpotLight(const UUID& uuid = 0);
		void destroyEntity(Entity entity);

		//TODO: debug purposes, remove
		void clear();
		void debugAllAvailableEntities();

		void enableSkybox(bool enabled);
		void changeSkybox();
	
		void enableReflection(bool enabled);
		void enableRefraction(bool enabled);
		
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
		bool m_isSkyboxEnabled = false;
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

} // namespace Rocket