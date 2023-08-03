#include "renderer2D.h"
#include <pch.h>
#include "renderer/vertex_array.h"
#include "renderer/shader.h"
#include "renderer/render_command.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Enik {

struct Renderer2DData {
	Ref<VertexArray> QuadVertexArray;
	Ref<Shader> FlatShader;

};

static Renderer2DData* s_Data;

void Renderer2D::Init() {
	s_Data = new Renderer2DData();

	s_Data->QuadVertexArray = VertexArray::Create();

	float vertices[4 * 3] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};


	Ref<VertexBuffer> vertexBuffer;
	vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
	
	
	BufferLayout layout = {
		{ShaderDataType::Float3, "a_Position"}
	};
	
	vertexBuffer->SetLayout(layout);
	s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
	
	Ref<IndexBuffer> indexBuffer;
	indexBuffer = IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t));
	s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

	s_Data->FlatShader = Shader::Create(FULL_PATH("assets/shaders/flat_color.glsl"));

}

void Renderer2D::Shutdown() {
	delete s_Data;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera) {
	s_Data->FlatShader->Bind();
	s_Data->FlatShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
}

void Renderer2D::EndScene() {

}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color) {
	DrawQuad({position.x, position.y, 0.0f}, scale, color);

}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color) {
	s_Data->FlatShader->Bind();
	s_Data->FlatShader->SetFloat4("u_Color", color);

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * 
		glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

	s_Data->FlatShader->SetMat4("u_Transform", transform);

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}

}