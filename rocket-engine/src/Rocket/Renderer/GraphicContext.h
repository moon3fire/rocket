#pragma once

namespace Rocket {
	class GraphicContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
} // namespace Rocket
