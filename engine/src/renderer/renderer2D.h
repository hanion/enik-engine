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
	static void Flush();

	static void DrawQuad(const Component::Transform& transform, const Component::SpriteRenderer& sprite, int32_t entityID = -1);

	struct Statistics {
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		uint32_t GetTotalVertexCount() { return QuadCount * 4; }
		uint32_t GetTotalIndexCount()  { return QuadCount * 6; }
	};

	static void ResetStats();
	static Statistics GetStats();

private:
	static void FlushAndReset();

	static float GetTextureIndex(const Ref<Texture2D>& texture);
};

}