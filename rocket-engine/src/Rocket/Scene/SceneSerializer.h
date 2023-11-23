#pragma once

#include "Scene.h"

namespace Rocket {

	class SceneSerializer {
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void serialize(const std::string& filepath);
		void serializeRuntime(const std::string& filepath);

		bool deserialize(const std::string& filepath);
		bool deserializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_scene;
	};

} // namespace Rocket