#include "rcktpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

#include "Entity.h"
#include "ScriptableEntity.h"
#include "Components.h"

#include "Rocket/Renderer/Renderer2D.h"

//temp
#include "Controller.h"

namespace Rocket {
	
	static b2BodyType RocketRBTypeToB2Type(RigidBody2DComponent::BodyType type) {
		switch (type) {
			case RigidBody2DComponent::BodyType::Static:		return b2_staticBody;
			case RigidBody2DComponent::BodyType::Dynamic:		return b2_dynamicBody;
			case RigidBody2DComponent::BodyType::Kinematic:		return b2_kinematicBody;
		}

		RCKT_CORE_ASSERT(false, "Unknown rigid body 2D type!");
		return b2_staticBody;
	}
	
	Scene::Scene() {}

	Scene::~Scene() {
		m_registry.clear();
	}

	template <typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src,const std::unordered_map<UUID, entt::entity>& enttMap) {
		
		auto view = src.view<Component>();
		for (auto e : view) {
			UUID uuid = src.get<TagComponent>(e).id;

			RCKT_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), "");
			
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);
		}
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other) {

		Ref<Scene> newScene = createRef<Scene>();

		newScene->m_viewportWidth = other->m_viewportWidth;
		newScene->m_viewportHeight = other->m_viewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = other->m_registry;
		auto& dstSceneRegistry = newScene->m_registry;
		auto view = srcSceneRegistry.view<TagComponent>();

		for (auto e : view) {
			auto& tc = srcSceneRegistry.get<TagComponent>(e);
			Entity entity = newScene->createEntity(tc.tag, tc.id);
			enttMap[tc.id] = (entt::entity)entity;
		}

		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<DirectionalLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<PointLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpotLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap); // ?
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<RigidBody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::createEntity(const std::string& name, const UUID& uuid) {
		Entity entity = { m_registry.create(), this }; 

		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>(name);
		auto& entityTag = entity.getComponent<TagComponent>();
		entityTag.tag = name.empty() ? "Unnamed" : name;
		entityTag.id = uuid;
		m_entityCount++;
		RCKT_CORE_TRACE("Entity count {0}, current Entity UUID: {1}", m_entityCount, uuid);
		return entity;
	}

	Entity Scene::createDirectionalLight(const UUID& uuid) {
		Entity entity = { m_registry.create(), this };

		entity.addComponent<TransformComponent>();
		entity.addComponent<TagComponent>("Directional Light " + std::to_string(m_directionalLightCount));
		entity.addComponent<DirectionalLightComponent>();
		auto& entityTag = entity.getComponent<TagComponent>();
		entityTag.id = uuid;

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
		auto& entityTag = entity.getComponent<TagComponent>();
		entityTag.id = uuid;

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
		auto& entityTag = entity.getComponent<TagComponent>();
		entityTag.id = uuid;

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
		m_registry.destroy(entity); // ???
	}

	void Scene::clear() {
		auto group = m_registry.group<TransformComponent>();
		for (auto& e : group) {
			Entity entity = { e, this };
			if (entity.hasComponent<RigidBody2DComponent>()) {
				auto& body = entity.getComponent<RigidBody2DComponent>().runtimeBody;
				delete body;
				entity.removeComponent<RigidBody2DComponent>();
			}
			if (entity.hasComponent<BoxCollider2DComponent>()) {
				auto& fixture = entity.getComponent<BoxCollider2DComponent>().runtimeFixture;
				delete fixture;
				entity.removeComponent<BoxCollider2DComponent>();
			}
			m_registry.destroy(entity); 
		}
		m_registry.clear();
		Renderer2D::reset();
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

	void Scene::onRuntimeStart() {
		m_physicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_registry.view<RigidBody2DComponent>();
		for (auto e : view) {
			Entity entity = { e, this };
			auto& tc = entity.getComponent<TransformComponent>();
			auto& rb2d = entity.getComponent<RigidBody2DComponent>();
			
			b2BodyDef bodyDef;
			bodyDef.type = RocketRBTypeToB2Type(rb2d.type);
			bodyDef.position.Set(tc.position.x, tc.position.y);
			bodyDef.angle = tc.rotation.z;
			b2Body* body = m_physicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.fixedRotation);
			rb2d.runtimeBody = body;

			if (entity.hasComponent<BoxCollider2DComponent>()) {
				auto& bc2d = entity.getComponent<BoxCollider2DComponent>();

				b2PolygonShape box;
				box.SetAsBox(bc2d.size.x * tc.scale.x, bc2d.size.y * tc.scale.y);

				b2FixtureDef fixture;
				fixture.shape = &box;
				fixture.density = bc2d.density;
				fixture.friction = bc2d.friction;
				fixture.restitution = bc2d.restitution;
				fixture.restitutionThreshold = bc2d.restitutionThreshold;
				body->CreateFixture(&fixture);
			}
		}
	}

	void Scene::onRuntimeStop() {
		delete m_physicsWorld;
		m_physicsWorld = nullptr;
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
			return; // return if there are no cameras in the scene
		}

		//physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_physicsWorld->Step(ts, velocityIterations, positionIterations);

			auto view = m_registry.view<RigidBody2DComponent>();
			for (auto e : view) {
				Entity entity = { e, this };
				auto& tc = entity.getComponent<TransformComponent>();
				auto& rb2d = entity.getComponent<RigidBody2DComponent>();

				b2Body* body = (b2Body*)rb2d.runtimeBody;
				const auto& position = body->GetPosition();
				tc.position.x = position.x;
				tc.position.y = position.y;
				tc.rotation.z = body->GetAngle();
			}
		}

		Camera* mainCamera = &mainCameraEntity.getComponent<CameraComponent>().camera;
		glm::mat4 cameraTransform = mainCameraEntity.getComponent<TransformComponent>().getTransform();
		

		if (mainCamera) {
			Renderer2D::beginScene(mainCamera->getProjection(), cameraTransform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			// draw squares
			{
				auto view = m_registry.view<SpriteRendererComponent, TransformComponent>();

				for (auto entity : view) {

					auto [sprite, transform] = view.get<SpriteRendererComponent, TransformComponent>(entity);
					Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
				}
			}
			// draw circles
			{
				auto view = m_registry.view<CircleRendererComponent, TransformComponent>();

				for (auto entity : view) {

					auto [circle, transform] = view.get<CircleRendererComponent, TransformComponent>(entity);
					Renderer2D::drawCircle(transform.getTransform(), circle, (int)entity);
				}
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera, const glm::vec2& viewportSize) {
		Renderer2D::beginScene(camera);
		/*
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
		*/
		
		// Note: View is read only, group is rw
		// draw squares
		{
			auto view = m_registry.view<SpriteRendererComponent, TransformComponent>();

			for (auto entity : view) {

				auto [sprite, transform] = view.get<SpriteRendererComponent, TransformComponent>(entity);
				Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
			}
		}
		// draw circles
		{
			auto view = m_registry.view<CircleRendererComponent, TransformComponent>();

			for (auto entity : view) {

				auto [circle, transform] = view.get<CircleRendererComponent, TransformComponent>(entity);
				Renderer2D::drawCircle(transform.getTransform(), circle, (int)entity);
			}
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
	void Scene::onComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component) {}

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

	template<>
	void Scene::onComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component) {}

	template<>
	void Scene::onComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component) {}

} // namespace Rocket