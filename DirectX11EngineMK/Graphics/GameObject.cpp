#include "GameObject.h"

const XMVECTOR& GameObject::GetPositionVector() const
{
	return _posVector;
}

const XMFLOAT3& GameObject::GetPositionFloat3() const
{
	return _pos;
}

const XMVECTOR& GameObject::GetRotationVector() const
{
	return _rotVector;
}

const XMFLOAT3& GameObject::GetRotationFloat3() const
{
	return _rot;
}

void GameObject::SetPosition(const XMVECTOR& pos)
{
	XMStoreFloat3(&_pos, pos);
	_posVector = pos;
	UpdateMatrix();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	_pos = pos;
	_posVector = XMLoadFloat3(&_pos);
	UpdateMatrix();
}

void GameObject::SetPosition(float x, float y, float z)
{
	_pos = XMFLOAT3(x, y, z);
	_posVector = XMLoadFloat3(&_pos);
	UpdateMatrix();
}

void GameObject::AdjustPosition(const XMVECTOR& pos)
{
	_posVector += pos;
	XMStoreFloat3(&_pos, _posVector);
	UpdateMatrix();
}

void GameObject::AdjustPosition(const XMFLOAT3& pos)
{
	_pos.x += pos.y;
	_pos.y += pos.y;
	_pos.z += pos.z;
	_posVector = XMLoadFloat3(&_pos);
	UpdateMatrix();
}

void GameObject::AdjustPosition(float x, float y, float z)
{
	_pos.x += x;
	_pos.y += y;
	_pos.z += z;
	_posVector = XMLoadFloat3(&_pos);
	UpdateMatrix();
}

void GameObject::SetRotation(const XMVECTOR& rot)
{
	_rotVector = rot;
	XMStoreFloat3(&_rot, rot);
	UpdateMatrix();
}

void GameObject::SetRotation(const XMFLOAT3& rot)
{
	_rot = rot;
	_rotVector = XMLoadFloat3(&_rot);
	UpdateMatrix();
}

void GameObject::SetRotation(float x, float y, float z)
{
	_rot = XMFLOAT3(x, y, z);
	_rotVector = XMLoadFloat3(&_rot);
	UpdateMatrix();
}

void GameObject::AdjustRotation(const XMVECTOR& rot)
{
	_rotVector += rot;
	XMStoreFloat3(&_rot, _rotVector);
	UpdateMatrix();
}

void GameObject::AdjustRotation(const XMFLOAT3& rot)
{
	_rot.x += rot.x;
	_rot.y += rot.y;
	_rot.z += rot.z;
	_rotVector = XMLoadFloat3(&_rot);
	UpdateMatrix();
}

void GameObject::AdjustRotation(float x, float y, float z)
{
	_rot.x += x;
	_rot.y += y;
	_rot.z += z;
	_rotVector = XMLoadFloat3(&_rot);
	UpdateMatrix();
}

void GameObject::SetScale(float xScale, float yScale, float zScale)
{
	_scale.x = xScale;
	_scale.y = yScale;
	_scale.z = zScale;
	UpdateMatrix();
}

void GameObject::UpdateMatrix()
{
	assert("UpdateMatrix must be overridden." && 0);
}


