#include "rcktpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Rocket {

	Ref<Shader> Shader::create(const std::string& filepath, const std::string& additionalFilepath) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:		RCKT_CORE_ASSERT(false, "RendererAPI not found: None");
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLShader>(filepath, additionalFilepath);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	Ref<Shader> Shader::create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:		RCKT_CORE_ASSERT(false, "RendererAPI not found: None");
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLShader>(name, vertexSource, fragmentSource);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader) {
		RCKT_CORE_ASSERT(!exists(name), "Shader already exists!");
		m_shaders[name] = shader;
	}

	void ShaderLibrary::add(const Ref<Shader>& shader) {
		auto& name = shader->getName();
		add(name, shader);
	}

	Ref<Shader> ShaderLibrary::load(const std::string& filepath) {
		auto shader = Shader::create(filepath);
		add(shader); 
		return shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath) {
		auto shader = Shader::create(filepath);
		add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::get(const std::string& name) {
		RCKT_CORE_ASSERT(exists(name), "Shader not found!");
		return m_shaders[name];
	}

	bool ShaderLibrary::exists(const std::string& name) const {
		return m_shaders.find(name) != m_shaders.end();
	}

} // namespace Rocket