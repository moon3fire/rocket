#pragma once

#include "Rocket/Scene/Scene.h"
#include "Rocket/Core/Log.h"
#include "Rocket/Scene/Entity.h"
#include "Rocket/Core/Core.h"
#include "Rocket/Renderer/Texture.h"

namespace Rocket {

	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel();
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void setContext(const Ref<Scene>& scene);

		void onImGuiRender();
		
		void setSelectedEntity(Entity entity = {});
		Entity getSelectedEntity() const { return m_selectionContext; }
	private:
		void drawEntityNode(Entity entity);
		void drawProperties(Entity entity);

	private:
		bool m_isHDREnabled = false;

		Ref<Texture2D> m_defaultTexture;
		Ref<Scene> m_context;
		Entity m_selectionContext;
	};

}