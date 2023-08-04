#pragma once
#include "renderer/shader.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Enik {

class OpenGLShader : public Shader {
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	virtual ~OpenGLShader();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	virtual void SetInt(const std::string& name, const int& value) override final;
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
private:
	std::string ReadFile(const std::string& filepath);
	std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

private:
	uint32_t m_RendererID;
	std::string m_Name;
};

}