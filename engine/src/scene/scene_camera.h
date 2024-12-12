#pragma once

#include <base.h>
#include "renderer/camera.h"

namespace Enik {

class SceneCamera : public Camera {
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

	void SetSize       (float _size ) { m_OrthographicSize = _size ; RecalculateProjection(); }
	void SetFar        (float _far  ) { m_OrthographicFar  = _far  ; RecalculateProjection(); }
	void SetNear       (float _near ) { m_OrthographicNear = _near ; RecalculateProjection(); }
	void SetAspectRatio(float _ratio) { m_AspectRatio      = _ratio; RecalculateProjection(); }

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