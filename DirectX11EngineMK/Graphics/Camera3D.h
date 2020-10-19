#pragma once
#include "GameObject3D.h"

class Camera3D : public GameObject3D
{
public:
	Camera3D();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;

private:
	void UpdateMatrix() override;

private:
	XMMATRIX _viewMatrix;
	XMMATRIX _projectionMatrix;
};