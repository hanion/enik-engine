#include "renderer2D.h"
#include <pch.h>
#include "renderer/vertex_array.h"
#include "renderer/shader.h"
#include "renderer/render_command.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Enik {


struct QuadVertex {
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
	float TileScale;
};

struct Renderer2DData {
	static const uint32_t MaxQuads = 10000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices  = MaxQuads * 6;
	static const uint32_t MaxTextureSlots  = 16;

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<Shader> TextureColorShader;
	Ref<Texture2D> WhiteTexture;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr  = nullptr;

	std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;	
	uint32_t TextureSlotIndex = 1;	
};

static Renderer2DData s_Data;


void Renderer2D::Init() {
	EN_PROFILE_SCOPE;

	s_Data.QuadVertexArray = VertexArray::Create();

	s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

	s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
	
	
	BufferLayout layout = {
		{ShaderDataType::Float3, "a_Position"},
		{ShaderDataType::Float4, "a_Color"},
		{ShaderDataType::Float2, "a_TexCoord"},
		{ShaderDataType::Float , "a_TexIndex"},
		{ShaderDataType::Float , "a_TileScale"}
	};
	
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

	s_Data.TextureColorShader = Shader::Create(FULL_PATH("assets/shaders/texture_color.glsl"));

	s_Data.WhiteTexture = Texture2D::Create(1,1);
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

void Renderer2D::BeginScene(const OrthographicCamera& camera) {
	EN_PROFILE_SCOPE;

	s_Data.TextureColorShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::EndScene() {
	EN_PROFILE_SCOPE;

	uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
	s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);	
	Flush();
}

void Renderer2D::Flush() {
	EN_PROFILE_SCOPE;

	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) {
		s_Data.TextureSlots[i]->Bind(i);
	}
	
	RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
}


void Renderer2D::FlushAndReset() {
	EndScene();

	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}



void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color) {
	DrawQuad({position.x, position.y, 0.0f}, scale, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color) {
	const float textureIndex = 0.0f; // white texture
	DrawQuad(position, scale, textureIndex, color);
}



void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const Ref<Texture2D>& texture) {
	DrawQuad({position.x, position.y, 0.0f}, scale, texture);
}
void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const Ref<Texture2D>& texture) {
	constexpr glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	DrawQuad(position, scale, texture, color);
}



void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color, float tileScale) {
	DrawQuad({position.x, position.y, 0.0f}, scale, texture, color, tileScale);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color, float tileScale) {
	EN_PROFILE_SECTION("Texture Index finder");

	float textureIndex = 0.0f;
	for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
		if (texture->eaquals(*s_Data.TextureSlots[i])) {
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f) {
		textureIndex = (float)s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	DrawQuad(position, scale, textureIndex, color, tileScale);
}


void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& scale, float textureIndex, const glm::vec4& color, float tileScale) {
	EN_PROFILE_SCOPE;

	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		FlushAndReset();
	}
	
	constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}}; 
	glm::vec3 positions[] = {
		position,
		{ position.x + scale.x, position.y          , 0.0f }, 
		{ position.x + scale.x, position.y + scale.y, 0.0f },
		{ position.x          , position.y + scale.y, 0.0f }
	}; 
	
	for (size_t i = 0; i < 4; i++) {
		s_Data.QuadVertexBufferPtr->Position = positions[i];
		s_Data.QuadVertexBufferPtr->Color = color;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TileScale = tileScale;
		s_Data.QuadVertexBufferPtr++;
	}
	
	s_Data.QuadIndexCount += 6;

}

}