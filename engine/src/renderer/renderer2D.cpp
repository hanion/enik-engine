#include "renderer2D.h"

#include <pch.h>

#include <glm/gtc/matrix_transform.hpp>

#include "renderer/render_command.h"
#include "renderer/shader.h"
#include "renderer/vertex_array.h"

namespace Enik {

struct QuadVertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
	float TileScale;

	// Editor only
	int a_EntityID;
};

struct LineVertex {
	glm::vec3 Position;
	glm::vec4 Color;
};


struct Renderer2DData {
	static const uint32_t MaxQuads = 10000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
	static const uint32_t MaxTextureSlots = 16;

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<Shader> TextureColorShader;
	Ref<Texture2D> WhiteTexture;

	Ref<VertexArray> LineVertexArray;
	Ref<VertexBuffer> LineVertexBuffer;
	Ref<Shader> LineShader;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	uint32_t LineVertexCount = 0;
	LineVertex* LineVertexBufferBase = nullptr;
	LineVertex* LineVertexBufferPtr = nullptr;

	static const glm::vec4 QuadVertexPositions[4];

	std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
	uint32_t TextureSlotIndex = 1;

	Renderer2D::Statistics Stats;
};

static Renderer2DData s_Data;

const glm::vec4 Renderer2DData::QuadVertexPositions[] = {
	glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
	glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
	glm::vec4(0.5f, 0.5f, 0.0f, 1.0f),
	glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)};

void Renderer2D::Init() {
	EN_PROFILE_SCOPE;

	s_Data.QuadVertexArray = VertexArray::Create();

	s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

	s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

	BufferLayout layout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float4, "a_Color"},
		{ShaderDataType::Float2, "a_TexCoord"},
		{ShaderDataType::Float, "a_TexIndex"},
		{ShaderDataType::Float, "a_TileScale"},
		{ShaderDataType::Int, "a_EntityID"}};

	s_Data.QuadVertexBuffer->SetLayout(layout);
	s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

	uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) {
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;

		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;

		offset += 4;
	}

	Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
	s_Data.QuadVertexArray->SetIndexBuffer(indexBuffer);
	delete[] quadIndices;


	// Lines
	s_Data.LineVertexArray = VertexArray::Create();
	s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];
	s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
	BufferLayout line_layout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float4, "a_Color"}};
	s_Data.LineVertexBuffer->SetLayout(line_layout);
	s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);

	s_Data.LineShader = Shader::Create(FULL_PATH_EDITOR("assets/shaders/line_shader.glsl"));
	s_Data.LineShader->Bind();


	s_Data.TextureColorShader = Shader::Create(FULL_PATH_EDITOR("assets/shaders/texture_color.glsl"));

	s_Data.WhiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTextureData = 0xffffffff;
	s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	s_Data.TextureSlots[0] = s_Data.WhiteTexture;
	s_Data.TextureSlots[0]->Bind();

	s_Data.TextureColorShader->Bind();
	int32_t samplers[s_Data.MaxTextureSlots];
	for (size_t i = 0; i < s_Data.MaxTextureSlots; i++) {
		samplers[i] = i;
	}
	s_Data.TextureColorShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);
}

void Renderer2D::Shutdown() {
}

void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform) {
	EN_PROFILE_SCOPE;

	glm::mat4 viewProjectionMatrix = camera.GetProjection() * glm::inverse(transform);

	s_Data.TextureColorShader->SetMat4("u_ViewProjection", viewProjectionMatrix);

	StartBatch();
}

void Renderer2D::BeginScene(const OrthographicCamera& camera) {
	EN_PROFILE_SCOPE;

	s_Data.TextureColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

	StartBatch();
}

void Renderer2D::EndScene() {
	EN_PROFILE_SCOPE;

	Flush();
}

void Renderer2D::StartBatch() {
	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.LineVertexCount = 0;
	s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::Flush() {
	EN_PROFILE_SCOPE;

	if (s_Data.QuadIndexCount) {
		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
			s_Data.TextureSlots[i]->Bind(i);
		}

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
		s_Data.Stats.DrawCalls++;
	}

	if (s_Data.LineVertexCount) {
		uint32_t data_size = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
		s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, data_size);
		RenderCommand::DrawLine(s_Data.LineVertexArray, s_Data.LineVertexCount);
		s_Data.Stats.DrawCalls++;
	}

}



float Renderer2D::GetTextureIndex(const Ref<Texture2D>& texture) {
	EN_PROFILE_SCOPE;

	if (texture == nullptr) {
		return 0.0f;
	}

	float textureIndex = 0.0f;
	for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
		if (texture->equals(*s_Data.TextureSlots[i])) {
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f) {
		textureIndex = (float)s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	return textureIndex;
}


void Renderer2D::DrawQuad(const Component::Transform& trans, const Component::SpriteRenderer& sprite, int32_t entityID) {
	EN_PROFILE_SCOPE;

	const glm::vec2 defaultTextureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
	const glm::vec2* textureCoords = defaultTextureCoords;

	float textureIndex = 0.0f;

	if (sprite.SubTexture) {
		textureIndex = GetTextureIndex(sprite.SubTexture->GetTexture());
		textureCoords = sprite.SubTexture->GetTextureCoords();
	}
	else {
		textureIndex = GetTextureIndex(sprite.Texture);
	}

	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		Flush();
		StartBatch();
	}

	glm::mat4 transform;
	glm::vec3 positions[4];

	if (trans.Rotation) {
		transform = trans.GetTransform();
	}
	else {
		glm::vec2 half_scale = trans.Scale / 2.0f;

		positions[0] = {trans.Position.x - half_scale.x, trans.Position.y - half_scale.y, trans.Position.z};
		positions[1] = {trans.Position.x + half_scale.x, trans.Position.y - half_scale.y, trans.Position.z};
		positions[2] = {trans.Position.x + half_scale.x, trans.Position.y + half_scale.y, trans.Position.z};
		positions[3] = {trans.Position.x - half_scale.x, trans.Position.y + half_scale.y, trans.Position.z};
	}

	for (size_t i = 0; i < 4; i++) {
		if (trans.Rotation) {
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
		}
		else {
			s_Data.QuadVertexBufferPtr->Position = positions[i];
		}
		s_Data.QuadVertexBufferPtr->Color = sprite.Color;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TileScale = sprite.TileScale;
		s_Data.QuadVertexBufferPtr->a_EntityID = entityID;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color) {
	DrawLine(glm::vec3(p0.x, p0.y, 0.99f), glm::vec3(p1.x, p1.y, 0.99f), color);
}

void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color) {
	s_Data.LineVertexBufferPtr->Position = p0;
	s_Data.LineVertexBufferPtr->Color = color;
	s_Data.LineVertexBufferPtr++;

	s_Data.LineVertexBufferPtr->Position = p1;
	s_Data.LineVertexBufferPtr->Color = color;
	s_Data.LineVertexBufferPtr++;

	s_Data.LineVertexCount += 2;
}

void Renderer2D::DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	DrawRect(glm::vec3(position.x, position.y, 0.99f), size, color);
}

void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
    glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
    glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
    glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
    glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

    DrawLine(p0, p1, color);
    DrawLine(p1, p2, color);
    DrawLine(p2, p3, color);
    DrawLine(p3, p0, color);
}

void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color) {
	glm::vec3 line_vertices[4];

	for (size_t i = 0; i < 4; i++) {
		line_vertices[i] = transform * s_Data.QuadVertexPositions[i];
	}

	DrawLine(line_vertices[0], line_vertices[1], color);
	DrawLine(line_vertices[1], line_vertices[2], color);
	DrawLine(line_vertices[2], line_vertices[3], color);
	DrawLine(line_vertices[3], line_vertices[0], color);
}

void Renderer2D::DrawRect(const Component::Transform& transform, const glm::vec4& color) {
	DrawRect(transform.GetTransform(), color);
}

void Renderer2D::DrawCircle(const glm::vec2& position, float radius, int segments, const glm::vec4& color) {
	float angle_increment = 2.0f * glm::pi<float>() / static_cast<float>(segments);
	glm::vec2 p0, p1;

	for (int i = 0; i < segments; i++) {
		float angle0 = i       * angle_increment;
		float angle1 = (i + 1) * angle_increment;

		p0.x = position.x + radius * glm::cos(angle0);
		p0.y = position.y + radius * glm::sin(angle0);

		p1.x = position.x + radius * glm::cos(angle1);
		p1.y = position.y + radius * glm::sin(angle1);

		DrawLine(p0, p1, color);
	}
}

void Renderer2D::ResetStats() {
	s_Data.Stats.DrawCalls = 0;
	s_Data.Stats.QuadCount = 0;
}

Renderer2D::Statistics Renderer2D::GetStats() {
	return s_Data.Stats;
}

}