#include "rcktpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>


namespace Rocket {

	static const std::filesystem::path s_assetsDir = "assets";

	ContentBrowserPanel::ContentBrowserPanel() :m_currentDir(s_assetsDir) {
		m_fileIcon = Texture2D::create("resources/icons/browser/rocketfile.png");
		m_dirIcon = Texture2D::create("resources/icons/browser/rocketfolder.png");
	}

	void ContentBrowserPanel::onImGuiRender() {

		ImGui::Begin("Content Browser");

		if (m_currentDir != std::filesystem::path(s_assetsDir)) {
			if (ImGui::Button("<-")) {
				m_currentDir = m_currentDir.parent_path();
			}
		}

		static float padding = 8.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& it : std::filesystem::directory_iterator(m_currentDir)) {
			
			const auto& path = it.path();
			auto relativePath = std::filesystem::relative(path, s_assetsDir);
			std::string filenameStr = relativePath.filename().string();
			
			if (it.is_directory()) {
				ImGui::ImageButton((ImTextureID)m_dirIcon->getRendererID(), { thumbnailSize, thumbnailSize },
					{ 0, 1 }, { 1, 0 }, -1, { 0.2f, 0.2f, 0.5f , 0.5f }, { 0.2f, 0.4f, 0.8f, 1.0f });
			}
			else {
				ImGui::ImageButton((ImTextureID)m_fileIcon->getRendererID(), { thumbnailSize, thumbnailSize },
				{0, 1}, {1, 0}, -1, { 0.2f, 0.2f, 0.5f , 0.5f }, { 0.3f, 0.3f, 0.5f, 0.7f });
			} // TODO: make a switch statement from this

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (it.is_directory())
					m_currentDir /= path.filename();
			}
			
			ImGui::Text(filenameStr.c_str());
			
			ImGui::NextColumn();
		}
		ImGui::Columns(1);

		ImGui::DragFloat("Thumbnail size", &thumbnailSize, 1, 64, 246);
		ImGui::DragFloat("Padding", &padding, 1, 0, 32);

		ImGui::End();


	}

} // namespace Rocket