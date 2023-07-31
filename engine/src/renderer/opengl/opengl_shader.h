#pragma once
#include "../shader.h"
#include <glm/glm.hpp>

namespace Enik {

class OpenglShader : public Shader {
public:
	OpenglShader(const std::string& vertexSource, const std::string& fragmentSource);
	virtual ~OpenglShader();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& vec);
private:
	uint32_t m_RendererID;
};

}