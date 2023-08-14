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

	static const glm::vec4 QuadVertexPositions[4];

	std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;	
	uint32_t TextureSlotIndex = 1;	

	Renderer2D::Statistics Stats;
};

static Renderer2DData s_Data;

const glm::vec4 Renderer2DData::QuadVertexPositions[] = {
    glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
    glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f),
    glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f),
    glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f)
};


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
	s_Data.Stats.DrawCalls++;
}


void Renderer2D::FlushAndReset() {
	EndScene();

	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}



float Renderer2D::GetTextureIndex(const Ref<Texture2D>& texture) {
    EN_PROFILE_SCOPE;

	if (texture == nullptr) { return 0.0f; }

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

	return textureIndex;
}


void Renderer2D::DrawQuad(const QuadProperties& quad) {
	EN_PROFILE_SCOPE;


	const glm::vec2 defaultTextureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };
	const glm::vec2* textureCoords = defaultTextureCoords;
	
	float textureIndex = 0.0f;

	if (quad.subTexture) {
		textureIndex = GetTextureIndex(quad.subTexture->GetTexture());
		textureCoords = quad.subTexture->GetTextureCoords();
	}
	else {
		textureIndex = GetTextureIndex(quad.texture);
	}


	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		FlushAndReset();
	}
	

	

	glm::mat4 transform;
	glm::vec3 positions[4];

	if (quad.rotation) {
		transform = glm::mat4(1.0f);
		transform = glm::translate(transform, quad.position)
			* glm::rotate(transform, quad.rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(transform, glm::vec3(quad.scale.x, quad.scale.y, 1.0f));
	}
	else {
		glm::vec2 half_scale = quad.scale/2.0f;

		positions[0] = { quad.position.x - half_scale.x, quad.position.y - half_scale.y, quad.position.z };
		positions[1] = { quad.position.x + half_scale.x, quad.position.y - half_scale.y, quad.position.z };
		positions[2] = { quad.position.x + half_scale.x, quad.position.y + half_scale.y, quad.position.z };
		positions[3] = { quad.position.x - half_scale.x, quad.position.y + half_scale.y, quad.position.z };
	}
	
	for (size_t i = 0; i < 4; i++) {
		if (quad.rotation) {
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
		}
		else {
			s_Data.QuadVertexBufferPtr->Position = positions[i];
		}
		s_Data.QuadVertexBufferPtr->Color = quad.color;
		s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		s_Data.QuadVertexBufferPtr->TileScale = quad.tileScale;
		s_Data.QuadVertexBufferPtr++;
	}
	
	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::ResetStats() {
	s_Data.Stats.DrawCalls = 0;
	s_Data.Stats.QuadCount = 0;
}

Renderer2D::Statistics Renderer2D::GetStats() {
	return s_Data.Stats;
}

}