#pragma once
#include "renderer/orthographic_camera.h"
#include "renderer/texture.h"
#include "renderer/sub_texture2D.h"
#include "scene/components.h"

namespace Enik {
namespace Renderer2D {

void Init();
void Shutdown();

void BeginScene(const Camera& camera, const glm::mat4& transform);
void BeginScene(const OrthographicCamera& camera);
void EndScene();
void StartBatch();
void Flush();

void DrawQuad(const Component::Transform& transform, const Component::SpriteRenderer& sprite, int32_t entityID = -1);
void DrawText2D(const Component::Transform& transform, const Component::Text& text, int32_t entityID = -1);

void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color, float thickness = 0.05f);
void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, float thickness = 0.05f);

void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float thickness = 0.05f);
void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float thickness = 0.05f);
void DrawRect(const glm::mat4& transform, const glm::vec4& color, float thickness = 0.05f);
void DrawRect(const Component::Transform& transform, const glm::vec4& color, float thickness = 0.05f);

void DrawCircle(const glm::vec2& position, float radius, int segments, const glm::vec4& color, float thickness = 0.05f);

struct Statistics {
	uint32_t DrawCalls = 0;
	uint32_t QuadCount = 0;

	uint32_t GetTotalVertexCount() { return QuadCount * 4; }
	uint32_t GetTotalIndexCount()  { return QuadCount * 6; }
};

void ResetStats();
Statistics GetStats();

float GetTextureIndex(const Ref<Texture2D>& texture);

void CreateErrorTexture();

const Ref<Texture2D> GetErrorTexture();

}
}