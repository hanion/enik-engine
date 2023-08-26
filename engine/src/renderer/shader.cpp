#include <pch.h>
#include "shader.h"
#include "renderer.h"
#include "opengl/opengl_shader.h"

namespace Enik {

Ref<Shader> Shader::Create(const std::string& filepath) {
	switch (Renderer::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(filepath);

		case RendererAPI::API::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}

Ref<Shader> Shader::Create(const std::string& name, const std::string& vertex_source, const std::string& fragment_source) {
	switch (Renderer::GetAPI()) {
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertex_source, fragment_source);

		case RendererAPI::API::None:
			EN_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;

		default:
			EN_CORE_ASSERT(false, "Unknown RendererAPI");
			return nullptr;
	}
}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
	EN_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end(), "Shader already exists!");
	m_Shaders[name] = shader;
}

void ShaderLibrary::Add(const Ref<Shader>& shader) {
	auto& name = shader->GetName();
	Add(name, shader);
}

Ref<Shader> ShaderLibrary::Load(const std::string& filepath) {
	auto shader = Shader::Create(filepath);
	Add(shader);
	return shader;
}
Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
	auto shader = Shader::Create(filepath);
	Add(name, shader);
	return shader;
}
Ref<Shader> ShaderLibrary::Get(const std::string& name) {
	EN_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Shader not found!");
	return m_Shaders[name];
}

}