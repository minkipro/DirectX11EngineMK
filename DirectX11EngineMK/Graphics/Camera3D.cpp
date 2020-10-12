#include "Camera3D.h"

Camera3D::Camera3D()
{
	_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_posVector = XMLoadFloat3(&_pos);
	_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_rotVector = XMLoadFloat3(&_rot);
	UpdateMatrix();
}

void Camera3D::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	_projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera3D::GetViewMatrix() const
{
	return _viewMatrix;
}

const XMMATRIX& Camera3D::GetProjectionMatrix() const
{
	return _projectionMatrix;
}

void Camera3D::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(_rot.x, _rot.y, _rot.z);
	XMVECTOR camTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	camTarget += _posVector;
	XMVECTOR upDir = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotationMatrix);
	_viewMatrix = XMMatrixLookAtLH(_posVector, camTarget, upDir);

	UpdateDirectionVectors();
}