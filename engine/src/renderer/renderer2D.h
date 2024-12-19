#pragma once
#include "renderer/orthographic_camera.h"
#include "renderer/texture.h"
#include "renderer/sub_texture2D.h"
#include "scene/components.h"

namespace Enik {

class Renderer2D {
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const Camera& camera, const glm::mat4& transform);
	static void BeginScene(const OrthographicCamera& camera);
	static void EndScene();
	static void StartBatch();
	static void Flush();

	static void DrawQuad(const Component::Transform& transform, const Component::SpriteRenderer& sprite, int32_t entityID = -1);
	static void DrawText2D(const Component::Transform& transform, const Component::Text& text, int32_t entityID = -1);

	static void DrawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color);
	static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color);

	static void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawRect(const glm::mat4& transform, const glm::vec4& color);
	static void DrawRect(const Component::Transform& transform, const glm::vec4& color);

	static void DrawCircle(const glm::vec2& position, float radius, int segments, const glm::vec4& color);

	struct Statistics {
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		uint32_t GetTotalVertexCount() { return QuadCount * 4; }
		uint32_t GetTotalIndexCount()  { return QuadCount * 6; }
	};

	static void ResetStats();
	static Statistics GetStats();

	static Ref<Texture2D> s_ErrorTexture;

private:
	static float GetTextureIndex(const Ref<Texture2D>& texture);

	static void CreateErrorTexture();
};

}