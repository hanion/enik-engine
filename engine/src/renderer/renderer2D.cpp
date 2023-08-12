#include "renderer2D.h"
#include <pch.h>
#include "renderer/vertex_array.h"
#include "renderer/shader.h"
#include "renderer/render_command.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Enik {

struct Renderer2DData {
	Ref<VertexArray> QuadVertexArray;
	Ref<Shader> TextureColorShader;
	Ref<Texture2D> WhiteTexture;

};

static Renderer2DData* s_Data;

void Renderer2D::Init() {
	EN_PROFILE_SCOPE;

	s_Data = new Renderer2DData();

	s_Data->QuadVertexArray = VertexArray::Create();

	float vertices[4 * 5] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};


	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	
	
	BufferLayout layout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float2, "a_TexCoord"}
	};
	
	vertexBuffer->SetLayout(layout);
	s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
	
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t));
	s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

	s_Data->TextureColorShader = Shader::Create(FULL_PATH("assets/shaders/texture_color.glsl"));

	s_Data->WhiteTexture = Texture2D::Create(1,1);
	uint32_t whiteTextureData = 0xffffffff;
	s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
}

void Renderer2D::Shutdown() {
	delete s_Data;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera) {
	EN_PROFILE_SCOPE;

	s_Data->TextureColorShader->Bind();
	s_Data->TextureColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	s_Data->TextureColorShader->SetInt("u_Texture", 0);

}

void Renderer2D::EndScene() {
	EN_PROFILE_SCOPE;


}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color) {
	DrawQuad({position.x, position.y, 0.0f}, scale, color);

}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color) {
	EN_PROFILE_SCOPE;

	s_Data->WhiteTexture->Bind();

	
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, position) * 
		glm::scale(transform, glm::vec3(scale.x, scale.y, 1.0f));

	s_Data->TextureColorShader->SetMat4("u_Transform", transform);
	s_Data->TextureColorShader->SetFloat4("u_Color", color);

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}



void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const Ref<Texture2D>& texture) {
	DrawQuad({position.x, position.y, 0.0f}, scale, texture);
}
void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const Ref<Texture2D>& texture) {
	EN_PROFILE_SCOPE;

	texture->Bind();

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, position) * 
		glm::scale(transform, glm::vec3(scale.x, scale.y, 1.0f));

	s_Data->TextureColorShader->SetMat4("u_Transform", transform);
	s_Data->TextureColorShader->SetFloat4("u_Color", glm::vec4(1.0f));

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}



void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color) {
	DrawQuad({position.x, position.y, 0.0f}, scale, texture, color);

}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color) {
	texture->Bind();

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, position) * 
		glm::scale(transform, glm::vec3(scale.x, scale.y, 1.0f));

	s_Data->TextureColorShader->SetMat4("u_Transform", transform);
	s_Data->TextureColorShader->SetFloat4("u_Color", color);
	s_Data->TextureColorShader->SetFloat("u_TileScale", 100.0f);

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}

}