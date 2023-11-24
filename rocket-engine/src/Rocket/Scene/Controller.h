#pragma once

#include "ScriptableEntity.h"
#include "Rocket/Core/Timestep.h"
#include "Components.h"
#include "Rocket/Core/KeyCodes.h"
#include "Rocket/Core/Input.h"

namespace Rocket {

	//TEMP
	class CameraController : public ScriptableEntity {
	public:
		void onCreate() {
			//getComponent<TransformComponent>();
		}

		void onDestroy() {

		}

		void onUpdate(Timestep ts) {
			auto& transform = getComponent<TransformComponent>().position;
			float speed = 5.0f;

			if (Input::isKeyPressed(RCKT_KEY_A)) {
				transform.x -= speed * ts;
			}
			if (Input::isKeyPressed(RCKT_KEY_W)) {
				transform.z -= speed * ts;
			}
			if (Input::isKeyPressed(RCKT_KEY_D)) {
				transform.x += speed * ts;
			}
			if (Input::isKeyPressed(RCKT_KEY_S)) {
				transform.z += speed * ts;
			}
			if (Input::isKeyPressed(RCKT_KEY_E)) {
				transform.y += speed * ts;
			}
			if (Input::isKeyPressed(RCKT_KEY_Q)) {
				transform.y -= speed * ts;
			}
		}
	};
	//TEMP ENDS HERE

} // namespace Rocket