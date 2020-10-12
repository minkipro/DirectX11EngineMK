#include "Camera2D.h"

Camera2D::Camera2D()
{
	_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_posVector = XMLoadFloat3(&_pos);
	_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_rotVector = XMLoadFloat3(&_rot);
	UpdateMatrix();
}

void Camera2D::SetProjectionValues(float width, float height, float nearZ, float farZ)
{
	_orthoMatrix = XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, nearZ, farZ);
}

const XMMATRIX& Camera2D::GetOrthoMatrix() const
{
	return _orthoMatrix;
}

const XMMATRIX& Camera2D::GetWorldMatrix() const
{
	return _worldMatrix;
}

void Camera2D::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	XMMATRIX translationOffsetMatrix = XMMatrixTranslation(-_pos.x, -_pos.y, 0.0f); //z component irrelevant for 2d camera
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(_rot.x, _rot.y, _rot.z);
	_worldMatrix = camRotationMatrix * translationOffsetMatrix;
}