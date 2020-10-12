#pragma once
#include "GameObject2D.h"

class Camera2D : public GameObject2D
{
public:
	Camera2D();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	const XMMATRIX& GetOrthoMatrix() const;
	const XMMATRIX& GetWorldMatrix() const;

private:
	void UpdateMatrix() override;

private:
	XMMATRIX _orthoMatrix;
	XMMATRIX _worldMatrix;
};