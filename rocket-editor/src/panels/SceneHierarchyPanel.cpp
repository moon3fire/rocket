#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Rocket/Scene/Components.h"

namespace Rocket {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) {
		setContext(scene);
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& scene) {
		m_context = scene;
		m_selectionContext = {};
	}

	void SceneHierarchyPanel::onImGuiRender() {
		
		{
			ImGui::Begin("Hierarchy");

			m_context->m_registry.each([&](auto entityID) {
				Entity entity{ entityID, m_context.get() };
				drawEntityNode(entity);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
				m_selectionContext = {};
			}

			// Right-Click on blank space
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
				if (ImGui::MenuItem("Create Empty")) {
					m_context->createEntity("Empty");
				}

				if (ImGui::MenuItem("Create Directional Light")) {
					m_context->createDirectionalLight();
				}

				ImGui::EndPopup();
			}


			ImGui::End();
		}

		{
			ImGui::Begin("Properties");

			if (m_selectionContext) {
				drawProperties(m_selectionContext);

			}

			ImGui::End();
		}
		
	}

	void SceneHierarchyPanel::setSelectedEntity(Entity entity) {
		m_selectionContext = entity;
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity) {
		auto& tag = entity.getComponent<TagComponent>().tag;
		ImGuiTreeNodeFlags flags = ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_selectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9848191, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted) {
			if (m_selectionContext == entity)
				m_selectionContext = {};

			m_context->destroyEntity(entity);
		}
	}

	template <typename T, typename UIFunction>
	static void drawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		if (entity.hasComponent<T>()) {
			auto& component = entity.getComponent<T>();
			ImVec2 contentRegion = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegion.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open) {
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.removeComponent<T>();
		}
	}

	static void DrawVec3Control(const std::string label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.9f, 0.2f, 0.25f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.25f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3f, 0.8f, 0.35f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.25f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.1f, 0.75f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.90f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.1f, 0.75f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	void SceneHierarchyPanel::drawProperties(Entity entity) {

		if (entity.hasComponent<TagComponent>()) {
			auto& tag = entity.getComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent")) {

			if (ImGui::MenuItem("Camera")) {
				m_selectionContext.addComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer")) {
				m_selectionContext.addComponent<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Camera Controller")) {
				m_context->addCameraController(m_selectionContext);
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		drawComponent<TransformComponent>("Transform", entity, [](auto& component) {
			DrawVec3Control("Position", component.position);
			DrawVec3Control("Scale", component.scale, 1.0f);
			glm::vec3 rotation = glm::degrees(component.rotation);
			DrawVec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);
			});

		drawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		});

		drawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component) {
			float strenght = component.ambientStrenght;
			ImGui::DragFloat("Ambient Strenght", &strenght, 0.01f, 0.0f, 1.0f, "%.1f");
			ImGui::DragFloat3("Direction", glm::value_ptr(component.direction), 0.01f, -1.0f, 1.0f, "%.1f");
			ImGui::DragFloat3("Ambient Color", glm::value_ptr(component.ambient), 0.01f, -1.0f, 1.0f, "%.1f");
			ImGui::DragFloat3("Diffuse", glm::value_ptr( component.diffuse), 0.01f, -1.0f, 1.0f, "%.1f");
			ImGui::DragFloat3("Specular", glm::value_ptr(component.specular), 0.01f, -1.0f, 1.0f, "%.1f");

			component.ambientStrenght = strenght;
		});

		drawComponent<CameraComponent>("Camera Component", entity, [](auto& component) {

			ImGui::Checkbox("Primary", &component.primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)component.camera.getProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {

				for (int i = 0; i < 2; i++) {
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];

					if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
						currentProjectionTypeString = projectionTypeStrings[i];
						component.camera.setProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (component.camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				float orthoSize = component.camera.getOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					component.camera.setOrthographicSize(orthoSize);

				float orthoNear = component.camera.getOrthographicNearClip();
				if (ImGui::DragFloat("Near Clip", &orthoNear))
					component.camera.setOrthographicNearClip(orthoNear);

				float orthoFar = component.camera.getOrthographicFarClip();
				if (ImGui::DragFloat("Far Clip", &orthoFar))
					component.camera.setOrthographicFarClip(orthoFar);
			}

			if (component.camera.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
				float perspectiveFov = glm::degrees(component.camera.getPerspectiveVerticalFOV());
				if (ImGui::DragFloat("FOV", &perspectiveFov))
					component.camera.setPerspectiveVerticalFOV(glm::radians(perspectiveFov));

				float perspectiveNear = component.camera.getPerspectiveNearClip();
				if (ImGui::DragFloat("Near Clip", &perspectiveNear))
					component.camera.setPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = component.camera.getPerspectiveFarClip();
				if (ImGui::DragFloat("Far Clip", &perspectiveFar))
					component.camera.setPerspectiveFarClip(perspectiveFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
			}
		});

	}

} // namespace Rocket