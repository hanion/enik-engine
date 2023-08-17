#pragma once

#include "renderer/camera.h"

namespace Enik {

class SceneCamera : public Camera {
public:
	SceneCamera();
	virtual ~SceneCamera();

	void SetOrthographic(float size, float near, float far);

	void SetViewportSize(uint32_t width, uint32_t height);

private:
	void RecalculateProjection();

private:
	float m_OrthographicSize =  10.0f;
	float m_OrthographicFar  =  1.0f;
	float m_OrthographicNear = -1.0f;
	
	float m_AspectRatio = 1.0f;

};

}