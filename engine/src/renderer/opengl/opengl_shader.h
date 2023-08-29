#pragma once
#include "renderer/shader.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <unordered_map>


namespace Enik {

class OpenGLShader : public Shader {
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& name, const std::string& vertex_source, const std::string& fragment_source);
	virtual ~OpenGLShader();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void SetInt(const std::string& name, const int& value) override final;
	virtual void SetIntArray(const std::string& name, const int* values, uint32_t count) override final;
	virtual void SetFloat(const std::string& name, const float& value) override final;
	virtual void SetFloat3(const std::string& name, const glm::vec3& value) override final;
	virtual void SetFloat4(const std::string& name, const glm::vec4& value) override final;
	virtual void SetMat4(const std::string& name, const glm::mat4& value) override final;

	virtual const std::string& GetName() const override { return m_Name; }

	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	void UploadUniformFloat(const std::string& name, const float& value);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& vec);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& vec);
	void UploadUniformInt(const std::string& name, const int& value);
	void UploadUniformIntArray(const std::string& name, const int* values, uint32_t count);
private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	void Compile(const std::unordered_map<GLenum, std::string>& shader_sources);
	GLint GetUniformLocation(const std::string& name) const;

private:
	uint32_t m_RendererID;
	std::string m_Name;
	mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;
};

}