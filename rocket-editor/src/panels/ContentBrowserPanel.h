#pragma once

#include "Rocket/Renderer/Texture.h"

// std
#include <filesystem>

namespace Rocket {

	class ContentBrowserPanel {
	public:
		ContentBrowserPanel();

		void onImGuiRender();
	private:
		std::filesystem::path m_currentDir;

		Ref<Texture2D> m_dirIcon, m_fileIcon;
	};

} // namespace Rocket