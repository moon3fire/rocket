#include "rcktpch.h"
#include "Rocket/Utils/Utils.h"

#include <stb_image.h>

namespace Rocket {

	namespace Utils {

		unsigned int APIUtils::loadCubemap(const std::vector<std::string>& faces) {
			unsigned int textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(false);
			
			//glEnable(GL_FRAMEBUFFER_SRGB);
			
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				//if (i != 0)
				//	stbi_set_flip_vertically_on_load(false);

				unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					/*
					size_t dataSize = static_cast<size_t>(width) * height * nrChannels;
					float* imageData = new float[dataSize];
					for (size_t i = 0; i < dataSize; ++i) {
						imageData[i] = static_cast<float>(data[i]) / 255.0f;
					}
					delete[] imageData;
					*/
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					stbi_image_free(data);
				}
				else
				{
					RCKT_CORE_ERROR("Cubemap texture failed to load at path: {0}", faces[i]);
					stbi_image_free(data);
				}
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			return textureID;
		}



	} // namespace Utils
} // namespace Rocket

