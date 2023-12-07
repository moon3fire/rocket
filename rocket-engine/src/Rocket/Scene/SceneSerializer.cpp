#include "rcktpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace YAML {

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
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "1231814128"; // TODO: Entity ID goes here

		if (entity.hasComponent<TagComponent>()) {
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tag = entity.getComponent<TagComponent>().tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

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

			out << YAML::Key << "Ambient" << YAML::Value << plc.ambient;
			out << YAML::Key << "Diffuse" << YAML::Value << plc.diffuse;
			out << YAML::Key << "Specular" << YAML::Value << plc.specular;
			out << YAML::Key << "Constant" << YAML::Value << plc.constant;
			out << YAML::Key << "Linear" << YAML::Value << plc.linear;
			out << YAML::Key << "Quadratic" << YAML::Value << plc.quadratic;
			out << YAML::Key << "AmbientStrenght" << YAML::Value << plc.ambientStrenght;

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
			return false;
		}

		if (!data[0]["Scene"]) {
			RCKT_CORE_WARN("Can't found a scene inside file");
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

				Entity deserializedEntity = m_scene->createEntity(name);
				
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

					cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.camera.setPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.primary = cameraComponent["Primary"].as<bool>();
					cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent) {
					auto& src = deserializedEntity.addComponent<SpriteRendererComponent>();
					src.color = spriteRendererComponent["Color"].as<glm::vec4>();
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
					pointLightComponent.ambient			= plc["Ambient"].as<glm::vec3>();
					pointLightComponent.diffuse			= plc["Diffuse"].as<glm::vec3>();
					pointLightComponent.specular		= plc["Specular"].as<glm::vec3>();
					pointLightComponent.constant		= plc["Constant"].as<float>();
					pointLightComponent.linear			= plc["Linear"].as<float>();
					pointLightComponent.quadratic		= plc["Quadratic"].as<float>();
					pointLightComponent.ambientStrenght = plc["AmbientStrenght"].as<float>();
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