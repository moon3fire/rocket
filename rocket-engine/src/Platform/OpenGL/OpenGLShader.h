#pragma once

#include "Rocket/Renderer/Shader.h"
#include <unordered_map>

#include <glad.h>

namespace Rocket {

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& filepath, const std::string& additionalFilepath = "");
		OpenGLShader(int mode, const std::string& filepath);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setFloat(const std::string& name, float value) override;
		virtual void setFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void setFloatArray(const std::string& name, float* values, uint32_t count) override;
		virtual void setMat4(const std::string& name, const glm::mat4& value) override;
		virtual void setInt(const std::string& name, int value) override;
		virtual void setIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void setBool(const std::string& name, bool value) override;

		virtual void setDirectionalLights(const std::vector<DirectionalLightComponent>& dirLightComponents) override;
		virtual void setPointLights(const std::vector<PointLightComponent>& pointLightComponents) override;
		virtual void setSpotLights(const std::vector<SpotLightComponent>& spotLightComponents) override;

		virtual const std::string& getName() const override { return m_name; }

		// TODO: the same about refactoring in Shader class
		void uploadUniformDirectionLight(const std::vector<DirectionalLightComponent>& dirLightComponents);
		void uploadUniformPointLight(const std::vector<PointLightComponent>& pointLightComponents);
		void uploadUniformSpotLight(const std::vector<SpotLightComponent>& spotLightComponents);

		void uploadUniformBool(const std::string& name, bool value);

		void uploadUniformInt(const std::string& name, int value);
		void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void uploadUniformFloat(const std::string& name, float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void uploadUniformFloatArray(const std::string& name, float* values, uint32_t count);

		void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		std::string readFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
		std::array<GLenum, 2> compile(const std::unordered_map<GLenum, std::string>& shaderSources);
		void link(const std::array<GLenum, 2>& mainShaderIDs);
		void compileAndLink(const std::string& source);

		GLint getUniformLocation(const std::string& name) const;

	private:
		mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;

		uint32_t m_rendererID;
		std::string m_name;
	};

} // namespace Rocket