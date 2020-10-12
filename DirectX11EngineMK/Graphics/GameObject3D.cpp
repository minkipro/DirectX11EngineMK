#include "GameObject3D.h"
void GameObject3D::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (lookAtPos.x == _pos.x && lookAtPos.y == _pos.y && lookAtPos.z == _pos.z)
		return;

	lookAtPos.x = _pos.x - lookAtPos.x;
	lookAtPos.y = _pos.y - lookAtPos.y;
	lookAtPos.z = _pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& GameObject3D::GetForwardVector(bool omitY)
{
	if (omitY)
		return _vec_backward_noY;
	return _vec_forward;
}

const XMVECTOR& GameObject3D::GetRightVector(bool omitY)
{
	if (omitY)
		return _vec_right_noY;
	return _vec_right;
}

const XMVECTOR& GameObject3D::GetBackwardVector(bool omitY)
{
	if (omitY)
		return _vec_backward_noY;
	return _vec_backward;
}

const XMVECTOR& GameObject3D::GetLeftVector(bool omitY)
{
	if (omitY)
		return _vec_left_noY;
	return _vec_left;
}

void GameObject3D::UpdateMatrix()
{
	assert("UpdateMatrix must be overridden." && 0);
}

void GameObject3D::UpdateDirectionVectors()
{
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(_rot.x, _rot.y, 0.0f);
	_vec_forward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	_vec_backward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	_vec_left = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	_vec_right = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);

	XMMATRIX vecRotationMatrixNoY = XMMatrixRotationRollPitchYaw(0.0f, _rot.y, 0.0f);
	_vec_forward_noY = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	_vec_backward_noY = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	_vec_left_noY = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	_vec_right_noY = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}