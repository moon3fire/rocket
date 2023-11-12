#include "rcktpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include "Entity.h"
#include "Components.h"

#include "Rocket/Renderer/Renderer2D.h"

namespace Rocket {
	
	Scene::Scene() {

		entt::entity entity = m_registry.create();
	}

	Scene::~Scene() {

	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = { m_registry.create(), this };

		entity.addComponent<TransformComponent>();
		auto& entityTag = entity.addComponent<TagComponent>();
		entityTag.tag = name.empty() ? "Unnamed" : name;
		
		return entity;
	}

	void Scene::onUpdate(Timestep ts) {

		// renderer 2D

		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;
		
		auto group = m_registry.view<TransformComponent, CameraComponent>();

		for (auto entity : group) {
			const auto& [foundTransform, foundCamera] = group.get<TransformComponent, CameraComponent>(entity);

			if (foundCamera.primary) {
				mainCamera = &foundCamera.camera;
				cameraTransform = &foundTransform.transform;

				break;
			}
		}

		if (mainCamera) {
			Renderer2D::beginScene(mainCamera->getProjection(), *cameraTransform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group) {
				const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::drawQuadWithViewMat(transform, sprite.color);
			}

			Renderer2D::endScene();
		}
	}

} // namespace Rocket