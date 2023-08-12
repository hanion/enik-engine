#pragma once
#include "renderer/orthographic_camera.h"
#include "renderer/texture.h"

namespace Enik {

class Renderer2D {
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const OrthographicCamera& camera);
	static void EndScene();
	static void Flush();

	// Primitives
	static void DrawQuad(const glm::vec2& position, const glm::vec2& scale, const glm::vec4& color);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color);
	static void DrawQuad(const glm::vec2& position, const glm::vec2& scale, const Ref<Texture2D>& texture);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const Ref<Texture2D>& texture);
	static void DrawQuad(const glm::vec2& position, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color, float tileScale = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color, float tileScale = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& scale, float textureIndex, const glm::vec4& color, float tileScale = 1.0f);


};


}