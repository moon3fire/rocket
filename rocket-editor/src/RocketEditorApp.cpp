#include "rcktpch.h"
#include "EditorLayer.h"

namespace Rocket {

	class RocketEditor : public Application {
	public:
		RocketEditor() : Application("Rocket Editor") {
			pushLayer(new EditorLayer());
		}

		~RocketEditor() {

		}
	};

	Application* createApplication() {
		return new RocketEditor();
	}

} // namespace Rocket