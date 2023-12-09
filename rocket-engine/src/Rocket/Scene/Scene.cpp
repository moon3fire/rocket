#include "rcktpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Entity.h"
#include "ScriptableEntity.h"
#include "Components.h"

#include "Rocket/Renderer/Renderer2D.h"

//temp
#include "Controller.h"

namespace Rocket {
	
	Scene::Scene() {}

	Scene::~Scene() {}

	Entity Scene::createEntity(const std::string& name, const UUID& uuid) {
		Entity entity = { m_registry.create(), this }; 

		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>(name);
		auto& entityTag = entity.getComponent<TagComponent>();
		entityTag.tag = name.empty() ? "Unnamed" : name;
		m_entityCount++;
		RCKT_CORE_WARN("Entity count {0}", m_entityCount);
		return entity;
	}

	Entity Scene::createDirectionalLight(const UUID& uuid) {
		Entity entity = { m_registry.create(), this };

		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>("Directional Light " + std::to_string(m_directionalLightCount));
		entity.addComponent<DirectionalLightComponent>();
		
		m_entityCount++;
		m_directionalLightCount++;

		RCKT_CORE_WARN("Entity count {0}", m_entityCount);
		RCKT_CORE_WARN("Directional light count: {0}", m_directionalLightCount);
		return entity;
	}

	Entity Scene::createPointLight(const UUID& uuid) {
		Entity entity = { m_registry.create(), this };

		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>("Point Light " + std::to_string(m_pointLightCount));
		auto& plc = entity.addComponent<PointLightComponent>();
		plc.position = &(entity.getComponent<TransformComponent>().position);

		m_entityCount++;

		RCKT_CORE_WARN("Entity count {0}", m_entityCount);
		RCKT_CORE_WARN("Point light count: {0}", m_pointLightCount);
		return entity;
	}

	Entity Scene::createSpotLight(const UUID& uuid)
	{
		Entity entity = { m_registry.create(), this };

		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>("Spot Light " + std::to_string(m_spotLightCount));
		auto& slc = entity.addComponent<SpotLightComponent>();
		slc.position = &(entity.getComponent<TransformComponent>().position);

		m_entityCount++;

		RCKT_CORE_WARN("Entity count {0}", m_entityCount);
		RCKT_CORE_WARN("Spot light count: {0}", m_spotLightCount);
		return entity;
	}

	void Scene::destroyEntity(Entity entity) {
		if (entity.hasComponent<DirectionalLightComponent>())
			m_directionalLightCount--;
		else if (entity.hasComponent<PointLightComponent>())
			m_pointLightCount--;
		m_registry.destroy(entity);
	}

	void Scene::clear() {
		m_registry.each([&](auto entity) {
			m_registry.destroy(entity);
		});
	}

	//for debugging purposes
	void Scene::debugAllAvailableEntities() {
		m_registry.each([](auto entity) {
			RCKT_CORE_INFO("{0}", (int)entity);
		});
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		m_viewportWidth = width;
		m_viewportHeight = height;

		//resizing non fixed aspect ratio cameras in the scene
		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& camera = view.get<CameraComponent>(entity);
			if (!camera.fixedAspectRatio) {
				camera.camera.setViewportSize(width, height);
			}
		}
	}

	void Scene::enableSkybox(bool enabled) {
		m_isSkyboxEnabled = enabled;
	}

	void Scene::changeSkybox() {
		Renderer2D::changeSkybox();
	}

	void Scene::enableReflection(bool enabled) {
		Renderer2D::enableReflection(enabled);
	}

	void Scene::enableRefraction(bool enabled) {
		Renderer2D::enableRefraction(enabled);
	}

	void Scene::onUpdateRuntime(Timestep ts) {
		// native script handling, will be used later
		/* 
		// Update native scripts
		{
			m_registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {

				// TODO: Move to scene play
				if (!nsc.m_instance) {
					nsc.m_instance = nsc.instantiateScript();
					nsc.m_instance->m_entity = Entity{ entity, this };
					nsc.m_instance->onCreate();
				}

				nsc.m_instance->onUpdate(ts);
			});

		}
		*/
		// renderer 2D
		Entity mainCameraEntity = getPrimaryCameraEntity();
		if (!mainCameraEntity) {
			Renderer2D::endScene();
			return;
		}
		Camera* mainCamera = &mainCameraEntity.getComponent<CameraComponent>().camera;
		glm::mat4 cameraTransform = mainCameraEntity.getComponent<TransformComponent>().getTransform();

		if (mainCamera) {
			Renderer2D::beginScene(mainCamera->getProjection(), cameraTransform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera, const glm::vec2& viewportSize) {
		Renderer2D::beginScene(camera);
		
		// lights calculation
		{
			std::vector<DirectionalLightComponent> directionalLights;
			//getting directional lights
			{
				auto directionalGroup = m_registry.group<DirectionalLightComponent>(entt::get<TransformComponent>);
				for (auto light : directionalGroup) {
					directionalLights.push_back(directionalGroup.get<DirectionalLightComponent>(light));
				}
			}

			std::vector<PointLightComponent> pointLights;
			//getting point lights
			{
				auto pointLightGroup = m_registry.group<PointLightComponent>(entt::get<TransformComponent>);
				for (auto light : pointLightGroup) {
					pointLights.push_back(pointLightGroup.get<PointLightComponent>(light));
				}
			}

			std::vector<SpotLightComponent> spotLights;
			//getting spot lights
			{
				auto spotLightGroup = m_registry.group<SpotLightComponent>(entt::get<TransformComponent>);
				for (auto light : spotLightGroup) {
					spotLights.push_back(spotLightGroup.get<SpotLightComponent>(light));
				}
			}
			//applying lights
			{
				Renderer2D::applyDirectionalLights(directionalLights, camera.getPosition());
				Renderer2D::applyPointLights(pointLights);
				Renderer2D::applySpotLights(spotLights);
			}
		}

		// Note: View is read only, group is rw
		auto view = m_registry.view<TransformComponent, SpriteRendererComponent>(entt::exclude<DirectionalLightComponent>);

		for (auto entity : view) {

			auto [sprite, transform] = view.get<SpriteRendererComponent, TransformComponent>(entity);
			Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
		}

		Renderer2D::endScene();
		if (m_isSkyboxEnabled)
			Renderer2D::applySkybox(camera);
	}

	Entity Scene::getPrimaryCameraEntity() {
		auto view = m_registry.view<CameraComponent>();

		for (auto entity : view) {
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.primary)
				return Entity(entity, this);
		}
		return {};
	}

	//TEMP TO DELETE, this is used for controlling the cameras directly
	void Scene::addCameraController(Entity& entity) { entity.addComponent<NativeScriptComponent>().bind<CameraController>(); }

	template <typename T>
	void Scene::onComponentAdded(Entity entity, T& component) {
		RCKT_CORE_ASSERT(false, "Added unknown component");
	}

	template<>
	void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component) {}

	template<>
	void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component) {
		component.camera.setViewportSize(m_viewportWidth, m_viewportHeight);
	}

	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component) {}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component) {}
	
	template<>
	void Scene::onComponentAdded<DirectionalLightComponent>(Entity entity, DirectionalLightComponent& component) {}

	template<>
	void Scene::onComponentAdded<PointLightComponent>(Entity entity, PointLightComponent& component) {
		component.position = &entity.getComponent<TransformComponent>().position;
		m_pointLightCount++;
	}

	template<>
	void Scene::onComponentAdded<SpotLightComponent>(Entity entity, SpotLightComponent& component) {
		component.position = &entity.getComponent<TransformComponent>().position;
		m_spotLightCount++;
	}

	template<>
	void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {}

} // namespace Rocket