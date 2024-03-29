#pragma once

#include "Rocket/Renderer/EditorCamera.h"
#include "Rocket/Core/Timestep.h"
#include "Rocket/Core/UUID.h"

#include <entt.hpp>

class b2World;

namespace Rocket {

	// forward decl (v znak uvajeniya legendi)
	class Entity;
	class SceneHierarchyPanel;

	class Scene {
	public:

		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity createEntity(const std::string& name = "", const UUID& uuid = 0);
		Entity createDirectionalLight(const UUID& uuid = 0);
		Entity createPointLight(const UUID& uuid = 0);
		Entity createSpotLight(const UUID& uuid = 0);
		void duplicateEntity(Entity& other);
		void destroyEntity(Entity entity);

		//TODO: debug purposes, remove
		void clear();
		void debugAllAvailableEntities();

		void enableSkybox(bool enabled);
		void changeSkybox();
	
		void enableReflection(bool enabled);
		void enableRefraction(bool enabled);
		
		void enableHDR(bool enabled);
		void setExposure(float exposure);

		void enableColliders(bool enabled);

		void onViewportResize(uint32_t width, uint32_t height);

		void onRuntimeStart();
		void onRuntimeStop();

		void onUpdateRuntime(Timestep ts);
		void onUpdateEditor(Timestep ts, EditorCamera& camera, const glm::vec2& viewportSize);

		void drawLights();

		Entity getPrimaryCameraEntity();

		template <typename... Components>
		auto getAllEntitiesViewWith() { return m_registry.view<Components...>(); }

	private:
		template <typename T>
		void onComponentAdded(Entity entity, T& component);
	
	private: // TODO: Move this into editor camera
		static void addCameraController(Entity& entity);
	private:
		entt::registry m_registry;
		b2World* m_physicsWorld = nullptr;

		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;
		
		int m_entityCount = 0;
		int m_directionalLightCount = 0, m_pointLightCount = 0, m_spotLightCount = 0;
		
		bool m_isSkyboxEnabled = false;

		bool m_isHDREnabled = false;
		float m_exposure = 1.0f;

		bool m_showColliders = false;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

} // namespace Rocket