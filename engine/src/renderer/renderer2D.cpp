#include "renderer2D.h"

#include <pch.h>

#include <glm/gtc/matrix_transform.hpp>

#include "asset/asset_manager.h"
#include "base.h"
#include "core/asserter.h"
#include "renderer/font.h"
#include "renderer/render_command.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
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

	Ref<Texture2D> ErrorTexture;
};

static Renderer2DData s_Data;

const glm::vec4 Renderer2DData::QuadVertexPositions[] = {
	glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f),
	glm::vec4(0.5f, -0.5f, 0.0f, 1.0f),
	glm::vec4(0.5f, 0.5f, 0.0f, 1.0f),
	glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f)};

void Renderer2D::Init() {
	EN_PROFILE_SCOPE;

	CreateErrorTexture();

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

	s_Data.LineShader = Shader::Create(Project::FindAssetPath("shaders/line_shader.glsl").string());
	s_Data.LineShader->Bind();


	s_Data.TextureColorShader = Shader::Create(Project::FindAssetPath("shaders/texture_color.glsl").string());

	TextureSpecification spec {1,1,ImageFormat::RGBA8,false};
	uint32_t white_texture_data = (uint32_t)0xffffffff;
	Buffer data = Buffer(&white_texture_data, sizeof(uint32_t));
	s_Data.WhiteTexture = Texture2D::Create(spec, data);

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
	delete[] s_Data.QuadVertexBufferBase;
	delete[] s_Data.LineVertexBufferBase;
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

	if (s_Data.TextureSlotIndex == s_Data.MaxTextureSlots) {
		Flush();
		StartBatch();
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
	if (!sprite.Handle) {return;}
	EN_VERIFY(sprite.Handle);

	constexpr glm::vec2 default_texture_coords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
	const glm::vec2* texture_coords = default_texture_coords;

	float texture_index = 0.0f;

	Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(sprite.Handle);
	if (!texture) {
		texture = Renderer2D::GetErrorTexture();
	}
	texture_index = GetTextureIndex(texture);
	if (sprite.SubTexture) {
		texture_coords = sprite.SubTexture->GetTextureCoords();
	}

	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		Flush();
		StartBatch();
	}

	glm::vec3 transformed_positions[4];
	glm::mat4 transform = trans.GetTransform();
	for (size_t i = 0; i < 4; i++) {
		transformed_positions[i] = transform * s_Data.QuadVertexPositions[i];
	}

	for (size_t i = 0; i < 4; i++) {
		s_Data.QuadVertexBufferPtr->Position = transformed_positions[i];
		s_Data.QuadVertexBufferPtr->Color = sprite.Color;
		s_Data.QuadVertexBufferPtr->TexCoord = texture_coords[i];
		s_Data.QuadVertexBufferPtr->TexIndex = texture_index;
		s_Data.QuadVertexBufferPtr->TileScale = sprite.TileScale;
		s_Data.QuadVertexBufferPtr->a_EntityID = entityID;
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawText2D(const Component::Transform& transform, const Component::Text& text, int32_t entityID) {
	EN_PROFILE_SCOPE;

	if (text.Data.empty() || !text.Font || !AssetManager::IsAssetHandleValid(text.Font)) {
		return;
	}
	EN_VERIFY(text.Font);

	Ref<FontAsset> font_asset = AssetManager::GetAsset<FontAsset>(text.Font);
	float texture_index = GetTextureIndex(font_asset->AtlasTexture);

	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices) {
		Flush();
		StartBatch();
	}

	glm::mat4 trans = transform.GetTransform();
	float scale = text.Scale * 0.001f;
	glm::vec2 start_pos = glm::vec2(0,-1) * (font_asset->TextHeight * scale);

	size_t draw_until = std::min(text.Data.size(), (size_t)(text.Data.size() * text.Visible));

	for (size_t i = 0; i < draw_until; ++i) {
		if (text.Data[i] == '\n') {
			start_pos.x = 0;
			start_pos.y -= scale * font_asset->TextHeight;
			continue;
		}
		int character = static_cast<unsigned char>(text.Data[i]) - 32;
		if (character < 0 || size_t(character) >= font_asset->Glyphs.size()) {
			continue;
		}

		const Glyph& glyph = font_asset->Glyphs[character];
		glm::vec2 positions[4] = {
			{start_pos.x + glyph.positions[0].x * scale, start_pos.y - glyph.positions[0].y * scale},
			{start_pos.x + glyph.positions[1].x * scale, start_pos.y - glyph.positions[1].y * scale},
			{start_pos.x + glyph.positions[2].x * scale, start_pos.y - glyph.positions[2].y * scale},
			{start_pos.x + glyph.positions[3].x * scale, start_pos.y - glyph.positions[3].y * scale}
		};

		for (size_t j = 0; j < 4; j++) {
			s_Data.QuadVertexBufferPtr->Position = trans * glm::vec4(positions[j], 0.0f, 1.0f);
			s_Data.QuadVertexBufferPtr->Color = text.Color;
			s_Data.QuadVertexBufferPtr->TexCoord = glyph.tex_coords[j];
			s_Data.QuadVertexBufferPtr->TexIndex = texture_index;
			s_Data.QuadVertexBufferPtr->TileScale = 1.0f;
			s_Data.QuadVertexBufferPtr->a_EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		start_pos.x += glyph.Advance * scale;

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}
}

void Renderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, float thickness) {
	DrawLine(glm::vec3(p0.x, p0.y, 0.99f), glm::vec3(p1.x, p1.y, 0.99f), color, thickness);
}

void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, float thickness) {
	glm::vec3 line_dir = glm::normalize(p1 - p0);
	glm::vec3 perp_dir = glm::normalize(glm::vec3(-line_dir.y, line_dir.x, 0.0f));

	glm::vec3 offset = perp_dir * (thickness * 0.5f);

	glm::vec3 quad[4];
	quad[0] = p0 - offset;
	quad[1] = p1 - offset;
	quad[2] = p1 + offset;
	quad[3] = p0 + offset;

	for (int i = 0; i < 4; ++i) {
		s_Data.LineVertexBufferPtr->Position = quad[i];
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr++;
		s_Data.LineVertexCount++;
	}
}

void Renderer2D::DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float thickness) {
	DrawRect(glm::vec3(position.x, position.y, 0.99f), size, color, thickness);
}

void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float thickness) {
    glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
    glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
    glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
    glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

    DrawLine(p0, p1, color, thickness);
    DrawLine(p1, p2, color, thickness);
    DrawLine(p2, p3, color, thickness);
    DrawLine(p3, p0, color, thickness);
}

void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, float thickness) {
	glm::vec3 line_vertices[4];

	for (size_t i = 0; i < 4; i++) {
		line_vertices[i] = transform * s_Data.QuadVertexPositions[i];
	}

	DrawLine(line_vertices[0], line_vertices[1], color, thickness);
	DrawLine(line_vertices[1], line_vertices[2], color, thickness);
	DrawLine(line_vertices[2], line_vertices[3], color, thickness);
	DrawLine(line_vertices[3], line_vertices[0], color, thickness);
}

void Renderer2D::DrawRect(const Component::Transform& transform, const glm::vec4& color, float thickness) {
	DrawRect(transform.GetTransform(), color, thickness);
}

void Renderer2D::DrawCircle(const glm::vec2& position, float radius, int segments, const glm::vec4& color, float thickness) {
	float angle_increment = 2.0f * glm::pi<float>() / static_cast<float>(segments);
	glm::vec2 p0, p1;

	for (int i = 0; i < segments; i++) {
		float angle0 = i       * angle_increment;
		float angle1 = (i + 1) * angle_increment;

		p0.x = position.x + radius * glm::cos(angle0);
		p0.y = position.y + radius * glm::sin(angle0);

		p1.x = position.x + radius * glm::cos(angle1);
		p1.y = position.y + radius * glm::sin(angle1);

		DrawLine(p0, p1, color, thickness);
	}
}

void Renderer2D::ResetStats() {
	s_Data.Stats.DrawCalls = 0;
	s_Data.Stats.QuadCount = 0;
}

Renderer2D::Statistics Renderer2D::GetStats() {
	return s_Data.Stats;
}

void Renderer2D::CreateErrorTexture() {
	static uint8_t _s_ErrorTextureData[2 * 2 * 4] = {
		0,   0,   0, 255,   255, 0, 255, 255,
		255, 0, 255, 255,     0, 0,   0, 255
	};
	TextureSpecification spec;
	spec.Width = 2;
	spec.Height = 2;
	spec.Format = ImageFormat::RGBA8;
	spec.MagFilterLinear = false;
	Buffer data;
	data.Data = _s_ErrorTextureData;
	data.Size = sizeof(_s_ErrorTextureData);
	s_Data.ErrorTexture = Texture2D::Create(spec, data);
}
const Ref<Texture2D> Renderer2D::GetErrorTexture() {
	return s_Data.ErrorTexture;
}

}