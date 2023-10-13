#pragma once

namespace Rocket {

	class GraphicsContext {

	public:
		
		virtual void init() = 0;
		virtual void swapBuffers() = 0;
	
	};

} // namespace Rocket
