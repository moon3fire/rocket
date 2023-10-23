#include "rcktpch.h"
#include "OpenGLShader.h"

#include <fstream>

namespace Rocket {

	static GLenum shaderTypeFromString(const std::string& type) {
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		RCKT_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath) :m_rendererID(0) {
		std::string shaderSource = readFile(filepath);
		auto sources = preProcess(shaderSource);
		compile(sources);

		// extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
		:m_rendererID(0), m_name(name) {
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		compile(sources);
	}

	OpenGLShader::~OpenGLShader() {
		glDeleteProgram(m_rendererID);
	}

	std::string OpenGLShader::readFile(const std::string& filepath) {
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else {
			RCKT_CORE_ERROR("Couldn't open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string& source) {
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLenght = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			RCKT_CORE_ASSERT(eol != std::string::npos, "Syntax error!");
			size_t begin = pos + typeTokenLenght + 1;
			std::string type = source.substr(begin, eol - begin);
			RCKT_CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specifier");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			RCKT_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error!");
			pos = source.find(typeToken, nextLinePos);
			shaderSources[shaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
		RCKT_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now!");
		GLuint program = glCreateProgram();
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDindex = 0;
		for (auto& kv : shaderSources) {
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE) {
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infolog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infolog[0]);

				glDeleteShader(shader);

				RCKT_CORE_ERROR("{0}", infolog.data());
				RCKT_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDindex++] = shader;
		}

		glLinkProgram(program);
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infolog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infolog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs) {
				glDeleteShader(id);
			}

			RCKT_CORE_ERROR("{0}", infolog.data());
			RCKT_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs) {
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_rendererID = program;
	}

	GLint OpenGLShader::getUniformLocation(const std::string& name) const {
		if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
			return m_uniformLocationCache[name];

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		if (location == -1) {
			RCKT_CORE_WARN("Uniform '{0}' not found!", name);
			return location;
		}
		m_uniformLocationCache[name] = location;
		return location;
	}

	void OpenGLShader::bind() const {
		glUseProgram(m_rendererID);
	}

	void OpenGLShader::unbind() const {
		glUseProgram(0);
	}

	void OpenGLShader::setFloat2(const std::string& name, const glm::vec2& value) {
		uploadUniformFloat2(name, value);
	}

	void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& value) {
		uploadUniformFloat3(name, value);
	}

	void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value) {
		uploadUniformFloat4(name, value);
	}

	void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value) {
		uploadUniformMat4(name, value);
	}

	void OpenGLShader::setInt(const std::string& name, int value) {
		uploadUniformInt(name, value);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, int value) {
		GLint location = getUniformLocation(name);
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformFloat(const std::string& name, float value) {
		GLint location = getUniformLocation(name);
		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value) {
		GLint location = getUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value) {
		GLint location = getUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value) {
		GLint location = getUniformLocation(name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix) {
		GLint location = getUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
		GLint location = getUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}
