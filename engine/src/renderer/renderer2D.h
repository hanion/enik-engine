#pragma once
#include "renderer/orthographic_camera.h"
#include "renderer/texture.h"

namespace Enik {

struct QuadProperties {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec2 scale    = glm::vec2(1.0f);
	glm::vec4 color    = glm::vec4(1.0f);
	
	Ref<Texture2D> texture = nullptr;
	float tileScale = 1.0f;
	float rotation = 0.0f;
};


class Renderer2D {
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const OrthographicCamera& camera);
	static void EndScene();
	static void Flush();

	// Primitives
	static void DrawQuad(const QuadProperties& quadProperties);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color, float tileScale = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, float textureIndex, const glm::vec4& color, float tileScale = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, float textureIndex, const glm::vec4& color, float tileScale, float rotation);


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