#pragma once
#include "Model.h"

class GameObject
{
public:
	const XMVECTOR& GetPositionVector() const;
	const XMFLOAT3& GetPositionFloat3() const;
	const XMVECTOR& GetRotationVector() const;
	const XMFLOAT3& GetRotationFloat3() const;

	void SetPosition(const XMVECTOR& pos);
	void SetPosition(const XMFLOAT3& pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const XMVECTOR& pos);
	void AdjustPosition(const XMFLOAT3& pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const XMVECTOR& rot);
	void SetRotation(const XMFLOAT3& rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const XMVECTOR& rot);
	void AdjustRotation(const XMFLOAT3& rot);
	void AdjustRotation(float x, float y, float z);
	void SetScale(float xScale, float yScale, float zScale = 1.0f);

protected:
	virtual void UpdateMatrix();
	

	XMVECTOR							_posVector;
	XMVECTOR							_rotVector;
	XMFLOAT3							_pos;
	XMFLOAT3							_rot;

	XMFLOAT3							_scale;
};