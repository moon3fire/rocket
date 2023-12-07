#include "rcktpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>


namespace Rocket {

	extern const std::filesystem::path g_assetsDir = "assets";

	ContentBrowserPanel::ContentBrowserPanel() :m_currentDir(g_assetsDir) {
		m_fileIcon = Texture2D::create("resources/icons/browser/rocketfile.png");
		m_dirIcon = Texture2D::create("resources/icons/browser/rocketfolder.png");
	}

	void ContentBrowserPanel::onImGuiRender() {

		ImGui::Begin("Content Browser");

		if (m_currentDir != std::filesystem::path(g_assetsDir)) {
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
			auto relativePath = std::filesystem::relative(path, g_assetsDir);
			std::string filenameStr = relativePath.filename().string();
			ImGui::PushID(filenameStr.c_str());


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.5f, 0.5f));
			if (it.is_directory()) {
				ImGui::ImageButton((ImTextureID)m_dirIcon->getRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			}
			else {
				ImGui::ImageButton((ImTextureID)m_fileIcon->getRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }, -1, { 0.2f, 0.2f, 0.2f, 0.2f }, { 0.2f, 0.7f, 0.5f, 0.8f });
			} // TODO: make a switch statement from this
			ImGui::PopStyleColor();

			if (path.extension().string() == ".rckt") {
				if (ImGui::BeginDragDropSource()) {
					ImVec2 mousePos = ImGui::GetMousePos();
					ImGui::GetForegroundDrawList()->AddImage((ImTextureID)m_fileIcon->getRendererID(), ImVec2(mousePos.x, mousePos.y + 64), ImVec2(mousePos.x + 64, mousePos.y));

					const wchar_t* itemPath = relativePath.c_str();
					ImGui::SetDragDropPayload("SCENE", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
					ImGui::EndDragDropSource();
				}
			}
			else if (path.extension().string() == ".png") {
				if (ImGui::BeginDragDropSource()) {
					ImVec2 mousePos = ImGui::GetMousePos();
					ImGui::GetForegroundDrawList()->AddImage((ImTextureID)m_fileIcon->getRendererID(), ImVec2(mousePos.x, mousePos.y + 64), ImVec2(mousePos.x + 64, mousePos.y));

					const wchar_t* itemPath = relativePath.c_str();
					ImGui::SetDragDropPayload("TEXTURE", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
					ImGui::EndDragDropSource();
				}
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (it.is_directory())
					m_currentDir /= path.filename();
			}
			
			ImGui::TextWrapped(filenameStr.c_str());
			
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);

		ImGui::DragFloat("Thumbnail size", &thumbnailSize, 1, 64, 246);
		ImGui::DragFloat("Padding", &padding, 1, 0, 32);

		// TODO: end status bar
		ImGui::End();
	}

} // namespace Rocket