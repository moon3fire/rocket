#pragma once

#include "Rocket/Scene/Scene.h"
#include "Rocket/Core/Log.h"
#include "Rocket/Scene/Entity.h"
#include "Rocket/Core/Core.h"

namespace Rocket {

	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void setContext(const Ref<Scene>& scene);

		void onImGuiRender();
	private:
		void drawEntityNode(Entity entity);
		void drawProperties(Entity entity);
	private:
		Ref<Scene> m_context;
		Entity m_selectionContext;
	};

}