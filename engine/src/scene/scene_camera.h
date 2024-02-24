#pragma once

#include <base.h>
#include "renderer/camera.h"

namespace Enik {

class EN_API SceneCamera : public Camera {
public:
	SceneCamera();
	virtual ~SceneCamera();

	void SetOrthographic(float size, float near, float far);

	void SetViewportSize(uint32_t width, uint32_t height);
	void SetViewportSize(glm::vec2 position, uint32_t width, uint32_t height);

	float GetSize       () { return m_OrthographicSize; }
	float GetFar        () { return m_OrthographicFar ; }
	float GetNear       () { return m_OrthographicNear; }
	float GetAspectRatio() { return m_AspectRatio     ; }

	void SetSize       (float size ) { m_OrthographicSize = size ; RecalculateProjection(); }
	void SetFar        (float far  ) { m_OrthographicFar  = far  ; RecalculateProjection(); }
	void SetNear       (float near ) { m_OrthographicNear = near ; RecalculateProjection(); }
	void SetAspectRatio(float ratio) { m_AspectRatio      = ratio; RecalculateProjection(); }

	glm::vec2 GetWorldPosition(const float x, const float y) const;

	glm::vec2 GetWorldPosition(const std::pair<float, float>& position) const {
		return GetWorldPosition(position.first , position.second);
	}

private:
	void RecalculateProjection();

private:
	float m_OrthographicSize =  10.0f;
	float m_OrthographicFar  =  1.0f;
	float m_OrthographicNear = -1.0f;

	float m_AspectRatio = 1.0f;

	float m_ScreenWidth  = 1.0f;
	float m_ScreenHeight = 1.0f;
	glm::vec2 m_ScreenPosition = glm::vec2(0.0f);

};

}