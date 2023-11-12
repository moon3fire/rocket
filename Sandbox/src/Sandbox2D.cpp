#include "rcktpch.h"
#include "Sandbox2D.h"

#include <Rocket/Core/EntryPoint.h>


static const char* s_mapTiles =
"677777777777777777777777777777777778"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXN3333333OXXXXXXX9"
"5XXXXXXXXXXXXXXXXXX9JJJJJJJ5XXXXXXX9"
"5XXXXXXXXXXXXXXXXXX9JJJJJJJ5XXXXXXX9"
"5XXXXXXXXXXXXXXXXXXM7777777PXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"5XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX9"
"123332333322333332223333333332233324"
;


Sandbox2D::Sandbox2D() :Layer("Sandbox2D"), m_cameraController(1280.0f / 720.0f, true) {
	onAttach();
}

void Sandbox2D::onAttach() {
	RCKT_PROFILE_FUNCTION();

	Rocket::FramebufferSpecification frameBufferSpec;
	frameBufferSpec.width = 1280;
	frameBufferSpec.height = 720;
	m_framebuffer = Rocket::Framebuffer::create(frameBufferSpec);

	m_spriteSheet = Rocket::Texture2D::create("assets/textures/spritesheet.png");
	m_textureMap['F'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 5, 5 }, { 64, 64 }); // press F to pay respect

	m_textureMap['X'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 12, 11 }, { 64, 64 }); // grass
	m_textureMap['A'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 12, 6 }, { 64, 64 }); // sand 
	m_textureMap['Q'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 11, 2 }, { 64, 64 }); // stone
	m_textureMap['J'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 0, 9 }, { 64, 64 }); // water

	//grass specific
	m_textureMap['1'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 10, 12 }, { 64, 64 }); // grass left corner
	m_textureMap['2'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 11, 12 }, { 64, 64 }); // grass corner next
	m_textureMap['3'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 10, 13 }, { 64, 64 }); // grass bot continue
	m_textureMap['4'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 11, 13 }, { 64, 64 }); // grass right corner
	m_textureMap['5'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 12, 10 }, { 64, 64 }); // grass left continue
	m_textureMap['6'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 13, 9 }, { 64, 64 }); // grass left up corner
	m_textureMap['7'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 13, 10 }, { 64, 64 }); // grass up continue
	m_textureMap['8'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 13, 11 }, { 64, 64 }); // grass up right corner
	m_textureMap['9'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 12, 12 }, { 64, 64 }); // grass right continue

	m_textureMap['M'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 12, 13 }, { 64, 64 }); // water-grass left bot
	m_textureMap['N'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 13, 12 }, { 64, 64 }); // water-grass left up
	m_textureMap['O'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 13, 13 }, { 64, 64 }); // water-grass right up
	m_textureMap['P'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 11, 0 }, { 64, 64 }); // water-grass right bot

	// additional textures on top of others
	m_textureMap['a'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 10, 1 }, { 64, 64 }); // tree 1
	m_textureMap['b'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 9, 2 }, { 64, 64 }); // tree 2
	m_textureMap['c'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 6, 9 }, { 64, 64 }); // mushroom
	m_textureMap['d'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 11, 10 }, { 64, 64 }); // cactus
	m_textureMap['e'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 5, 11 }, { 64, 64 }); // zarosli
	m_textureMap['f'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 10, 10 }, { 64, 64 }); // stone
	m_textureMap['g'] = Rocket::SubTexture2D::createFromCoords(m_spriteSheet, { 9, 1 }, { 64, 64 }); // tup
	
	m_mapWidth = 36;
	m_mapHeight = 20;
	
	for (int i = 0; i < 50; i++) {
		float x = rand() % m_mapWidth;
		float y = rand() % m_mapHeight;
		char tileType = 97 + rand() % 7;

		glm::vec2 scale = { 1.0f, 1.0f };
		/*
		* if needed
		if (tileType == 'a') {
			scale.x = 1.0f;
			scale.y = 1.0f;
		}
		*/

		m_subtextureData[i] = { { x, y }, scale, tileType };
	}

	m_cameraController.setZoomLevel(5.5f);
	m_cameraController.getBounds();
}

void Sandbox2D::onDetach() {
	RCKT_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Rocket::Timestep ts) {	
	RCKT_PROFILE_FUNCTION();

	//update
	
	m_cameraController.onUpdate(ts);
	
	//render
	Rocket::Renderer2D::resetStats();
	{
		RCKT_PROFILE_SCOPE("Clear color set:");
		Rocket::RenderCommand::setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		Rocket::RenderCommand::clear();
	}
	{
		RCKT_PROFILE_SCOPE("Scene render");
		Rocket::Renderer2D::beginScene(m_cameraController.getCamera());
		{

			for (uint32_t y = 0; y < m_mapHeight; y++) {
				for (uint32_t x = 0; x < m_mapWidth; x++) {
					char tileType = s_mapTiles[x + y * m_mapWidth];
					Rocket::Ref<Rocket::SubTexture2D> texture;
					if (m_textureMap.find(tileType) != m_textureMap.end())
						texture = m_textureMap[tileType];
					else
						texture = m_textureMap['F'];
					
					if (tileType > 47 && tileType < 58 || tileType > 76 && tileType < 81) {
						Rocket::Renderer2D::drawQuad3DWithSubTexture({ x - m_mapWidth / 2.0f, m_mapHeight - y - m_mapHeight / 2.0f, -0.9f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, m_textureMap['J']);
					}
					Rocket::Renderer2D::drawQuad3DWithSubTexture({ x - m_mapWidth / 2.0f, m_mapHeight - y - m_mapHeight / 2.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, texture, texture->getRotation());
				}
			}

			for (int i = 0; i < 50; i++) {
				Rocket::Ref<Rocket::SubTexture2D> texture;
				if (m_textureMap.find(m_subtextureData[i].tileSymbol) != m_textureMap.end())
					texture = m_textureMap[m_subtextureData[i].tileSymbol];
				else
					texture = m_textureMap['F'];

				Rocket::Renderer2D::drawQuad3DWithSubTexture({ m_subtextureData[i].coords.x - m_mapWidth / 2.0f, m_mapHeight - m_subtextureData[i].coords.y - m_mapHeight / 2.0f, 0.1f}, m_subtextureData[i].scale, {1.0f, 1.0f, 1.0f, 1.0f}, texture, texture->getRotation());
			}

		}
		Rocket::Renderer2D::endScene();
	}
}

void Sandbox2D::onImGuiRender() {
	RCKT_PROFILE_FUNCTION();

		ImGui::Begin("Settings");

		auto stats = Rocket::Renderer2D::getStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		uint32_t textureID = m_framebuffer->getColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ 1280, 720 });
		ImGui::End();
}

void Sandbox2D::onEvent(Rocket::Event& event) {
	m_cameraController.onEvent(event);
}
