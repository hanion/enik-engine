#pragma once
#include "renderer/shader.h"
#include <glm/glm.hpp>

namespace Enik {

class OpenGLShader : public Shader {
public:
	OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource);
	virtual ~OpenGLShader();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& vec);
private:
	uint32_t m_RendererID;
};

}