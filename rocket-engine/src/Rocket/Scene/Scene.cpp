#include "rcktpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Entity.h"
#include "Components.h"

#include "Rocket/Renderer/Renderer2D.h"

//temp
#include "Controller.h"

namespace Rocket {
	
	Scene::Scene() {}

	Scene::~Scene() {}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = { m_registry.create(), this }; 

		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>(name);
		auto& entityTag = entity.getComponent<TagComponent>();
		entityTag.tag = name.empty() ? "Unnamed" : name;
		m_entityCount++;
		RCKT_CORE_WARN("Entity count {0}", m_entityCount);
		return entity;
	}

	void Scene::destroyEntity(Entity entity) {
		m_registry.destroy(entity);
	}

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

	void Scene::onUpdateRuntime(Timestep ts) {

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

		// renderer 2D

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		
		auto view = m_registry.view<TransformComponent, CameraComponent>();

		for (auto entity : view) {
			auto [foundTransform, foundCamera] = view.get<TransformComponent, CameraComponent>(entity);

			if (foundCamera.primary) {
				mainCamera = &foundCamera.camera;
				cameraTransform = foundTransform.getTransform();

				break;
			}
		}

		if (mainCamera) {
			Renderer2D::beginScene(mainCamera->getProjection(), cameraTransform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				//Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
				//Renderer2D::drawQuadWithViewMat(transform.getTransform(), sprite.color);
				//Renderer2D::drawQuad2D({ 1.0f, 1.0f }, { 2.0f, 2.0f }, { 0.2f, 0.3f, 0.8f, 1.0f });
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera, const glm::vec3& diffusePos, const glm::vec3& diffuseColor) {
		Renderer2D::beginScene(camera);

		Renderer2D::uploadDiffuseLight(diffuseColor, diffusePos);

		auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
			//Renderer2D::uploadModelMatrix(transform.getTransform()); very temporary
			//Renderer2D::drawQuadWithViewMat(transform.getTransform(), sprite.color);
		}

		Renderer2D::endScene();
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

	//TEMP TO DELETE
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
	void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component) {}

} // namespace Rocket