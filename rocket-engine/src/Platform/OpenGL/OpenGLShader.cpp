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

	OpenGLShader::OpenGLShader(const std::string& filepath, const std::string& additionalFilepath) :m_rendererID(0) {
		RCKT_PROFILE_FUNCTION();


		// extracting name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_name = filepath.substr(lastSlash, count);

		// compilation and linking
		m_rendererID = glCreateProgram();
		
		RCKT_CORE_ASSERT(glIsProgram(m_rendererID), "Could not create OpenGL program!");

		std::string shaderSource = readFile(filepath);
		std::unordered_map<GLenum, std::string> sources = preProcess(shaderSource);
		std::unordered_map<GLenum, std::string> additionalSources;

		std::array<GLenum, 2> mainShaderIDs = { GL_FALSE };

		if (additionalFilepath != "") {
			RCKT_CORE_INFO("Received addition shader {0} with main shader {1}", additionalFilepath, m_name);
			std::string additional = readFile(additionalFilepath);
			additionalSources = preProcess(additional);

			for (auto it1 = sources.begin(), it2 = additionalSources.begin(); it1 != sources.end() && it2 != additionalSources.end(); ++it1, ++it2) {
				it1->second += it2->second;
			}
		}

		mainShaderIDs = compile(sources);
		
		link(mainShaderIDs);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
		:m_rendererID(0), m_name(name) {
		RCKT_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;
		compile(sources);
		// TODO: complete the functionality(it has been changed due to adding additional shader attachment)
	}

	OpenGLShader::~OpenGLShader() {
		RCKT_PROFILE_FUNCTION();

		glDeleteProgram(m_rendererID);
	}

	std::string OpenGLShader::readFile(const std::string& filepath) {
		RCKT_PROFILE_FUNCTION();

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
		RCKT_PROFILE_FUNCTION();

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

	std::array<GLenum, 2> OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
		RCKT_PROFILE_FUNCTION();

		RCKT_CORE_ASSERT(shaderSources.size() <= 2, "The API only support 2 shaders for now!"); // UPD: think about adding geometry shaders
		RCKT_CORE_ASSERT(glIsProgram(m_rendererID), "Could not create a program!");
		
		std::array<GLenum, 2> glShaderIDs = { 0 };
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

			glAttachShader(m_rendererID, shader);
			glShaderIDs[glShaderIDindex++] = shader;
		}

		return glShaderIDs;
	}

	void OpenGLShader::link(const std::array<GLenum, 2>& mainShaderIDs) {
		glLinkProgram(m_rendererID);
		GLint isLinked = 0;
		glGetProgramiv(m_rendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(m_rendererID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infolog(maxLength);
			glGetProgramInfoLog(m_rendererID, maxLength, &maxLength, &infolog[0]);

			glDeleteProgram(m_rendererID);

			for (auto id : mainShaderIDs) {
				glDeleteShader(id);
			}

			RCKT_CORE_ERROR("{0}", infolog.data());
			RCKT_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : mainShaderIDs) {
			glDetachShader(m_rendererID, id);
			glDeleteShader(id);
		}
	}

	GLint OpenGLShader::getUniformLocation(const std::string& name) const {
		RCKT_PROFILE_FUNCTION();

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
		RCKT_PROFILE_FUNCTION();

		glUseProgram(m_rendererID);
	}

	void OpenGLShader::unbind() const {
		RCKT_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::setFloat(const std::string& name, float value) {
		uploadUniformFloat(name, value);
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

	void OpenGLShader::setFloatArray(const std::string& name, float* values, uint32_t count) {
		uploadUniformFloatArray(name, values, count);
	}

	void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value) {
		uploadUniformMat4(name, value);
	}

	void OpenGLShader::setInt(const std::string& name, int value) {
		uploadUniformInt(name, value);
	}

	void OpenGLShader::setIntArray(const std::string& name, int* values, uint32_t count) {
		// TODO: Implement, need to fix maybe
		uploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::setBool(const std::string& name, bool value) {
		uploadUniformBool(name, value);
	}

	void OpenGLShader::setDirectionalLights(const std::vector<DirectionalLightComponent>& dirLightComponents) {
		uploadUniformDirectionLight(dirLightComponents);
	}

	void OpenGLShader::setPointLights(const std::vector<PointLightComponent>& pointLightComponents) {
		uploadUniformPointLight(pointLightComponents);
	}

	void OpenGLShader::setSpotLights(const std::vector<SpotLightComponent>& spotLightComponents) {
		uploadUniformSpotLight(spotLightComponents);
	}

	void OpenGLShader::uploadUniformDirectionLight(const std::vector<DirectionalLightComponent>& dirLightComponents) {
		
		size_t lightsCount = dirLightComponents.size();
		RCKT_CORE_ASSERT(lightsCount <= SCENE_MAX_DIRECTIONAL_LIGHTS_COUNT, "Currently supported directional lights count in the scene is <= 10");
		
		// setting active lights count
		uploadUniformInt("u_directionalLightCount", lightsCount);
		
		if (lightsCount == 0) {
			return;
		}

		// setting strenghts float array to quad shader
		float strengths[SCENE_MAX_DIRECTIONAL_LIGHTS_COUNT];
		for (int i = 0; i < lightsCount; i++) {
			strengths[i] = dirLightComponents[i].ambientStrenght;
		}
		uploadUniformFloatArray("u_ambientStrenghtsDirectional", strengths, lightsCount);
		
		// setting directional lights uniforms		
		glm::vec3 directions[SCENE_MAX_DIRECTIONAL_LIGHTS_COUNT];
		glm::vec3 ambients[SCENE_MAX_DIRECTIONAL_LIGHTS_COUNT];
		glm::vec3 diffuses[SCENE_MAX_DIRECTIONAL_LIGHTS_COUNT];
		glm::vec3 speculars[SCENE_MAX_DIRECTIONAL_LIGHTS_COUNT];

		for (int i = 0; i < lightsCount; i++) {
			directions[i] = dirLightComponents[i].direction;
			ambients[i]   = dirLightComponents[i].ambient;
			diffuses[i]   = dirLightComponents[i].diffuse;
			speculars[i]  = dirLightComponents[i].specular;
	
			GLint directionLocation = getUniformLocation("directionals[" + std::to_string(i) + "].direction");
			GLint ambientLocation   = getUniformLocation("directionals[" + std::to_string(i) + "].ambient");
			GLint diffuseLocation   = getUniformLocation("directionals[" + std::to_string(i) + "].diffuse");
			GLint specularLocation  = getUniformLocation("directionals[" + std::to_string(i) + "].specular");

			glUniform3fv(directionLocation, 1, glm::value_ptr(directions[i]));
			glUniform3fv(ambientLocation, 1, glm::value_ptr(ambients[i]));
			glUniform3fv(diffuseLocation, 1, glm::value_ptr(diffuses[i]));
			glUniform3fv(specularLocation, 1, glm::value_ptr(speculars[i]));
		}
	}

	void OpenGLShader::uploadUniformPointLight(const std::vector<PointLightComponent>& pointLightComponents) {
		
		size_t lightsCount = pointLightComponents.size();
		RCKT_CORE_ASSERT(lightsCount <= SCENE_MAX_POINT_LIGHTS_COUNT, "Currently supported point lights count in the scene is <= 100");
		
		// setting active lights count
		uploadUniformInt("u_pointLightCount", lightsCount);

		if (lightsCount == 0) {
			return;
		}

		glm::vec3 positions[SCENE_MAX_POINT_LIGHTS_COUNT];
		glm::vec3 colors[SCENE_MAX_POINT_LIGHTS_COUNT];
		glm::vec3 ambients[SCENE_MAX_POINT_LIGHTS_COUNT];
		float intensities[SCENE_MAX_POINT_LIGHTS_COUNT];
		float radiuses[SCENE_MAX_POINT_LIGHTS_COUNT];
		
		for (int i = 0; i < lightsCount; i++) {
			positions[i]	= *pointLightComponents[i].position;
			colors[i]		= pointLightComponents[i].color;
			ambients[i]		= pointLightComponents[i].ambient;
			intensities[i]	= pointLightComponents[i].intensity;
			radiuses[i]		= pointLightComponents[i].radius;
	
			GLint positionLocation   = getUniformLocation("pointLights[" + std::to_string(i) + "].position");
			GLint colorLocation		 = getUniformLocation("pointLights[" + std::to_string(i) + "].color");
			GLint ambientLocation    = getUniformLocation("pointLights[" + std::to_string(i) + "].ambient");
			GLint intensityLocation  = getUniformLocation("pointLights[" + std::to_string(i) + "].intensity");
			GLint radiusLocation     = getUniformLocation("pointLights[" + std::to_string(i) + "].radius");

			glUniform3fv(positionLocation, 1, glm::value_ptr(positions[i]));
			glUniform3fv(colorLocation, 1, glm::value_ptr(colors[i]));
			glUniform3fv(ambientLocation, 1, glm::value_ptr(ambients[i]));
			glUniform1fv(intensityLocation, 1, &intensities[i]);
			glUniform1fv(radiusLocation, 1, &radiuses[i]);
		}
	}

	void OpenGLShader::uploadUniformSpotLight(const std::vector<SpotLightComponent>& spotLightComponents) {
		size_t lightsCount = spotLightComponents.size();
		RCKT_CORE_ASSERT(lightsCount <= SCENE_MAX_POINT_LIGHTS_COUNT, "Currently supported spot lights count in the scene is <= 100");

		// setting active lights count
		uploadUniformInt("u_spotLightCount", lightsCount);

		if (lightsCount == 0) {
			return;
		}

		glm::vec3 positions[SCENE_MAX_SPOT_LIGHTS_COUNT];
		glm::vec3 ambients[SCENE_MAX_SPOT_LIGHTS_COUNT];
		glm::vec3 diffuses[SCENE_MAX_SPOT_LIGHTS_COUNT];
		glm::vec3 speculars[SCENE_MAX_SPOT_LIGHTS_COUNT];
		float cutOffs[SCENE_MAX_SPOT_LIGHTS_COUNT];
		float outerCutOffs[SCENE_MAX_SPOT_LIGHTS_COUNT];
		float constants[SCENE_MAX_SPOT_LIGHTS_COUNT];
		float linears[SCENE_MAX_SPOT_LIGHTS_COUNT];
		float quadratics[SCENE_MAX_SPOT_LIGHTS_COUNT];

		for (int i = 0; i < lightsCount; i++) {
			positions[i]	= *spotLightComponents[i].position;
			ambients[i]		= spotLightComponents[i].ambient;
			diffuses[i]		= spotLightComponents[i].diffuse;
			speculars[i]	= spotLightComponents[i].specular;
			cutOffs[i]		= spotLightComponents[i].cutOff;
			outerCutOffs[i] = spotLightComponents[i].outerCutOff;
			constants[i]	= spotLightComponents[i].constant;
			linears[i]		= spotLightComponents[i].linear;
			quadratics[i]	= spotLightComponents[i].quadratic;

			GLint positionLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].position");
			GLint ambientLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].ambient");
			GLint diffuseLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].diffuse");
			GLint specularLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].specular");
			GLint cutOffsLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].cutOff");
			GLint outerCutOffsLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].outerCutOff");
			GLint constantsLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].constant");
			GLint linearsLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].linear");
			GLint quadraticsLocation = getUniformLocation("spotLights[" + std::to_string(i) + "].quadratic");

			glUniform3fv(positionLocation, 1, glm::value_ptr(positions[i]));
			glUniform3fv(ambientLocation, 1, glm::value_ptr(ambients[i]));
			glUniform3fv(diffuseLocation, 1, glm::value_ptr(diffuses[i]));
			glUniform3fv(specularLocation, 1, glm::value_ptr(speculars[i]));
			glUniform1fv(cutOffsLocation, 1, &cutOffs[i]);
			glUniform1fv(outerCutOffsLocation, 1, &outerCutOffs[i]);
			glUniform1fv(constantsLocation, 1, &constants[i]);
			glUniform1fv(linearsLocation, 1, &linears[i]);
			glUniform1fv(quadraticsLocation, 1, &quadratics[i]);
		}
	}

	void OpenGLShader::uploadUniformBool(const std::string& name, bool value) {
		GLint location = getUniformLocation(name);
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, int value) {
		GLint location = getUniformLocation(name);
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count) {
		GLint location = getUniformLocation(name);
		glUniform1iv(location, count, values);
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

	void OpenGLShader::uploadUniformFloatArray(const std::string& name, float* values, uint32_t count) {
		GLint location = getUniformLocation(name);
		glUniform1fv(location, count, values);
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
