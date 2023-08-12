#include "opengl_shader.h"
#include <pch.h>
#include "core/log.h"

#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


namespace Enik {

static GLenum ShaderTypeFromString(const std::string& type) {
	if (type == "vertex") {
		return GL_VERTEX_SHADER;
	}
	else if (type == "fragment") {
		return GL_FRAGMENT_SHADER;
	}
	
	EN_CORE_ASSERT(false, "Unknown shader type!");
	return 0;
}


OpenGLShader::OpenGLShader(const std::string& filepath) {
	EN_PROFILE_SCOPE;

	std::string source = ReadFile(filepath);
	auto shaderSources = PreProcess(source);
	Compile(shaderSources);

	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
	auto lastDot = filepath.rfind(".");

	auto count = (lastDot == std::string::npos) ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);

}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) 
	: m_Name(name) {

	EN_PROFILE_SCOPE;

	std::unordered_map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER] = vertexSource;
	sources[GL_FRAGMENT_SHADER] = fragmentSource;
	Compile(sources);
}

OpenGLShader::~OpenGLShader() {
	EN_PROFILE_SCOPE;

	glDeleteProgram(m_RendererID);
}



std::string OpenGLShader::ReadFile(const std::string& filepath) {
	EN_PROFILE_SCOPE;

    std::ifstream in = std::ifstream(filepath, std::ios::in | std::ios::binary);
	std::string result;

	if (in) {
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else {
		EN_CORE_ERROR("Could not open file '{0}'", filepath);
	}
	return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source) {
	EN_PROFILE_SCOPE;

	std::unordered_map<GLenum, std::string> shaderSources;
	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0);
	
	while (pos != std::string::npos) {
		size_t eol = source.find_first_of("\r\n",pos);
		EN_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + typeTokenLength + 1;
		std::string type = source.substr(begin, eol - begin);
		EN_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type!");

		size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		pos = source.find(typeToken,nextLinePos);

		size_t until = pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos);
		shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, until);
	}

	return shaderSources;
}

void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
	EN_PROFILE_SCOPE;

	
	GLuint program = glCreateProgram();

	EN_CORE_ASSERT(shaderSources.size() <= 2, "Only 2 shaders supported for now!");
	std::array<GLenum, 2> glShaderIDs;
	int glShaderIDIndex = 0;

	for (auto kv : shaderSources) {
		GLenum type = kv.first;
		std::string& source = kv.second;

		GLuint shader = glCreateShader(type);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar *sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		// Compile the vertex shader
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
			
			// We don't need the shader anymore.
			glDeleteShader(shader);

			EN_CORE_ERROR("{0}", infoLog.data());
			EN_CORE_ASSERT(false, "Shader compilation failure!");
			break;
		}
		glAttachShader(program, shader);
		glShaderIDs[glShaderIDIndex] = shader;
		glShaderIDIndex++;
	}

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		
		// We don't need the program anymore.
		glDeleteProgram(program);
		// Don't leak shaders either.
		for (auto id : glShaderIDs) {
			glDeleteShader(id);
		}
		EN_CORE_ERROR("{0}", infoLog.data());
		EN_CORE_ASSERT(false, "Shader link failure!");
		return;
	}

	// Always detach shaders after a successful link.
	for (auto id : glShaderIDs) {
		glDetachShader(program, id);
		glDeleteShader(id);
	}
	
	m_RendererID = program;
}

void OpenGLShader::Bind() const {
	EN_PROFILE_SCOPE;

	glUseProgram(m_RendererID);
}


void OpenGLShader::Unbind() const {
	glUseProgram(0);
}

void OpenGLShader::SetInt(const std::string& name, const int& value) {
	UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string& name, const int* values, uint32_t count) {
	UploadUniformIntArray(name, values, count);
}

void OpenGLShader::SetFloat(const std::string& name, const float& value) {
	UploadUniformFloat(name, value);
}

void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value) {
	UploadUniformFloat3(name, value);
}

void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value) {
	UploadUniformFloat4(name, value);
}

void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) {
	UploadUniformMat4(name, value);
}



void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) {
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformFloat(const std::string& name, const float& value) {
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vec) {
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vec) {
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform3f(location, vec.x, vec.y, vec.z);
}

void OpenGLShader::UploadUniformInt(const std::string& name, const int& value) {
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(const std::string& name, const int* values, uint32_t count) {
	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	glUniform1iv(location, count, values);
}

}