#include "rcktpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace YAML {

	template<>
	struct convert<glm::vec2> {

		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}

	};

	template<>
	struct convert<glm::vec3> {

		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}

	};

	template<>
	struct convert<glm::vec4> {

		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}

	};
} // namespace YAML

namespace Rocket {

	static std::string RigidBody2DTypeToString(RigidBody2DComponent::BodyType type) {
		switch (type) {
			case RigidBody2DComponent::BodyType::Static: return "Static";
			case RigidBody2DComponent::BodyType::Dynamic: return "Dynamic";
			case RigidBody2DComponent::BodyType::Kinematic: return "Kinematic";
		}
		RCKT_CORE_ASSERT(false, "Unknown rigid body 2D type!");
		return "Static";
	}

	static RigidBody2DComponent::BodyType StringToRigidBody2DType(const std::string& typeStr) {
		if (typeStr == "Static") return RigidBody2DComponent::BodyType::Static;
		if (typeStr == "Dynamic") return RigidBody2DComponent::BodyType::Dynamic;
		if (typeStr == "Kinematic") return RigidBody2DComponent::BodyType::Kinematic;

		RCKT_CORE_ASSERT(false, "Unknown type of rigid body 2D!");
		return RigidBody2DComponent::BodyType::Static;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2 v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3 v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4 v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene) :m_scene(scene) {}

	static void serializeEntity(YAML::Emitter& out, Entity entity) {
		RCKT_CORE_ASSERT(entity.hasComponent<TagComponent>(), "Couldn't serialize entity without unique identifier!");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.getUUID(); // TODO: Entity ID goes here

		if (entity.hasComponent<TagComponent>()) {
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tc = entity.getComponent<TagComponent>();

			out << YAML::Key << "Tag" << YAML::Value << tc.tag;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& tc = entity.getComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << tc.position;
			out << YAML::Key << "Scale" << YAML::Value << tc.scale;
			out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";

			out << YAML::BeginMap; // camera component

			auto& cc = entity.getComponent<CameraComponent>();
			auto& camera = cc.camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.getProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.getPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.getPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.getPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.getOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.getOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.getOrthographicFarClip();

			out << YAML::EndMap; // camera 

			out << YAML::Key << "Primary" << YAML::Value << cc.primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.fixedAspectRatio;

			out << YAML::EndMap; // camera component
		}

		if (entity.hasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& src = entity.getComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << src.color;
			out << YAML::Key << "Texture" << YAML::Value << src.getTexturePath();
			out << YAML::Key << "TilingFactor" << YAML::Value << src.tilingFactor;
			
			out << YAML::EndMap;
		}

		if (entity.hasComponent<CircleRendererComponent>()) {
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& crc = entity.getComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << crc.color;
			out << YAML::Key << "Thickness" << YAML::Value << crc.thickness;
			out << YAML::Key << "Fade" << YAML::Value << crc.fade;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<DirectionalLightComponent>()) {
			out << YAML::Key << "DirectionalLightComponent";
			out << YAML::BeginMap;

			auto& dlc = entity.getComponent<DirectionalLightComponent>();
			out << YAML::Key << "Ambient" << YAML::Value << dlc.ambient;
			out << YAML::Key << "AmbientStrenght" << YAML::Value << dlc.ambientStrenght;
			out << YAML::Key << "Diffuse" << YAML::Value << dlc.diffuse;
			out << YAML::Key << "Direction" << YAML::Value << dlc.direction;
			out << YAML::Key << "Specular" << YAML::Value << dlc.specular;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<PointLightComponent>()) {
			out << YAML::Key << "PointLightComponent";
			out << YAML::BeginMap;

			auto& plc = entity.getComponent<PointLightComponent>();

			out << YAML::Key << "Color" << YAML::Value << plc.color;
			out << YAML::Key << "Ambient" << YAML::Value << plc.ambient;
			out << YAML::Key << "Intensity" << YAML::Value << plc.intensity;
			out << YAML::Key << "Radius" << YAML::Value << plc.radius;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<RigidBody2DComponent>()) {
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap;

			auto& rb2d = entity.getComponent<RigidBody2DComponent>();

			out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DTypeToString(rb2d.type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.fixedRotation;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<BoxCollider2DComponent>()) {
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;

			auto& bc2d = entity.getComponent<BoxCollider2DComponent>();

			out << YAML::Key << "Offset" << YAML::Value << bc2d.offset;
			out << YAML::Key << "Size" << YAML::Value << bc2d.size;
			out << YAML::Key << "Density" << YAML::Value << bc2d.density;
			out << YAML::Key << "Friction" << YAML::Value << bc2d.friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2d.restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2d.restitutionThreshold;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<CircleCollider2DComponent>()) {
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& cc2d = entity.getComponent<CircleCollider2DComponent>();

			out << YAML::Key << "Offset" << YAML::Value << cc2d.offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2d.radius;
			out << YAML::Key << "Density" << YAML::Value << cc2d.density;
			out << YAML::Key << "Friction" << YAML::Value << cc2d.friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2d.restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2d.restitutionThreshold;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::serialize(const std::string& filepath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::EndMap;
		out << YAML::BeginMap;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_scene->m_registry.each([&](auto entityID) {
			Entity entity = { entityID, m_scene.get() };
			if (!entity)
				return;

			serializeEntity(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::serializeRuntime(const std::string& filepath) {
		//not implemented
		RCKT_CORE_ASSERT(false, "");
	}

	bool SceneSerializer::deserialize(const std::string& filepath) {
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		std::vector<YAML::Node> data;
		try
		{
			data = YAML::LoadAllFromFile(filepath);
		}
		catch(YAML::ParserException e)
		{
			RCKT_CORE_ERROR("{0}", e.what());
			return false;
		}

		if (!data[0]["Scene"]) {
			RCKT_CORE_WARN("Can't find a scene inside file");
			return false;
		}
		RCKT_CORE_INFO("Found scene: {0}", data[0].begin()->second.Scalar());

		auto entities = data[1]["Entities"];

		if (entities) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO
				RCKT_CORE_INFO(uuid);
				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent) {
					name = tagComponent["Tag"].as<std::string>();
				}

				RCKT_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_scene->createEntity(name, uuid);
				
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent) {
					// transform component always exists
					auto& tc = deserializedEntity.getComponent<TransformComponent>();
					tc.position = transformComponent["Position"].as<glm::vec3>();
					tc.scale = transformComponent["Scale"].as<glm::vec3>();
					tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
				}
				else {
					RCKT_CORE_ASSERT(false, "Scene file is broken! (Entity does not have a transform component!)");;
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					auto& cc = deserializedEntity.addComponent<CameraComponent>();
					auto cameraProps = cameraComponent["Camera"];

					SceneCamera::ProjectionType type = (SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>();

					cc.camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.primary = cameraComponent["Primary"].as<bool>();
					cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
					cc.camera.setOnlyProjectionType(type);
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent) {
					auto& src = deserializedEntity.addComponent<SpriteRendererComponent>();
					src.color = spriteRendererComponent["Color"].as<glm::vec4>();
					std::string texturePath = spriteRendererComponent["Texture"].as<std::string>();
					if (texturePath != "")
						src.texture = Texture2D::create(texturePath);
					src.tilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent) {
					auto& crc = deserializedEntity.addComponent<CircleRendererComponent>();
					crc.color = circleRendererComponent["Color"].as<glm::vec4>();
					crc.thickness = circleRendererComponent["Thickness"].as<float>();
					crc.fade = circleRendererComponent["Fade"].as<float>();
				}

				auto dlc = entity["DirectionalLightComponent"];
				if (dlc) {
					auto& directionalLightComponent = deserializedEntity.addComponent<DirectionalLightComponent>();
					directionalLightComponent.ambient = dlc["Ambient"].as<glm::vec3>();
					directionalLightComponent.ambientStrenght = dlc["AmbientStrenght"].as<float>();
					directionalLightComponent.diffuse = dlc["Diffuse"].as<glm::vec3>();
					directionalLightComponent.direction = dlc["Direction"].as<glm::vec3>();
					directionalLightComponent.specular = dlc["Specular"].as<glm::vec3>();
				}

				auto plc = entity["PointLightComponent"];
				if (plc) {
					auto& pointLightComponent = deserializedEntity.addComponent<PointLightComponent>();
					auto& tc = deserializedEntity.getComponent<TransformComponent>();
					pointLightComponent.position		= &tc.position;
					pointLightComponent.color			= plc["Color"].as<glm::vec3>();
					pointLightComponent.ambient			= plc["Ambient"].as<glm::vec3>();
					pointLightComponent.intensity		= plc["Intensity"].as<float>();
					pointLightComponent.radius			= plc["Radius"].as<float>();
				}

				auto rb2d = entity["RigidBody2DComponent"];
				if (rb2d) {
					auto& rigidBody2DComponent = deserializedEntity.addComponent<RigidBody2DComponent>();
					rigidBody2DComponent.type = StringToRigidBody2DType(rb2d["BodyType"].as<std::string>());
					rigidBody2DComponent.fixedRotation = rb2d["FixedRotation"].as<bool>();
				}

				auto bc2d = entity["BoxCollider2DComponent"];
				if (bc2d) {
					auto& boxCollider2DComponent = deserializedEntity.addComponent<BoxCollider2DComponent>();
					boxCollider2DComponent.offset				= bc2d["Offset"].as<glm::vec2>();
					boxCollider2DComponent.size					= bc2d["Size"].as<glm::vec2>();
					boxCollider2DComponent.density				= bc2d["Density"].as<float>();
					boxCollider2DComponent.friction				= bc2d["Friction"].as<float>();
					boxCollider2DComponent.restitution			= bc2d["Restitution"].as<float>();
					boxCollider2DComponent.restitutionThreshold = bc2d["RestitutionThreshold"].as<float>();
				}

				auto cc2d = entity["CircleCollider2DComponent"];
				if (cc2d) {
					auto& circleCollider2DComponent = deserializedEntity.addComponent<CircleCollider2DComponent>();
					circleCollider2DComponent.offset				= cc2d["Offset"].as<glm::vec2>();
					circleCollider2DComponent.radius				= cc2d["Radius"].as<float>();
					circleCollider2DComponent.density				= cc2d["Density"].as<float>();
					circleCollider2DComponent.friction				= cc2d["Friction"].as<float>();
					circleCollider2DComponent.restitution			= cc2d["Restitution"].as<float>();
					circleCollider2DComponent.restitutionThreshold	= cc2d["RestitutionThreshold"].as<float>();
				}
			}
		}

		return true;

	}

	bool SceneSerializer::deserializeRuntime(const std::string& filepath) {
		//not implemented
		RCKT_CORE_ASSERT(false, "");
		return false;
	}

} // namespace Rocket