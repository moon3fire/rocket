#pragma once

#include <glm/glm.hpp>

#include "Texture.h"

namespace Rocket {

	class SubTexture2D {
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D> getTexture() const { return m_texture; }
		const glm::vec2* getTexCoords() const { return m_texCoords; }

		float getRotation() const { return m_rotation; }
		void setRotation(float rotation) { m_rotation = rotation; }

		static Ref<SubTexture2D> createFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1, 1 });
	private:
		Ref<Texture2D> m_texture;

		float m_rotation;
		glm::vec2 m_texCoords[4];
	};

} // namespace Rocket