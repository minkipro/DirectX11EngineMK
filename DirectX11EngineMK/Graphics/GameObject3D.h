#pragma once
#include "GameObject.h"

class GameObject3D : public GameObject
{
public:
	void SetLookAtPos(XMFLOAT3 lookAtPos);
	const XMVECTOR& GetForwardVector(bool omitY = false);
	const XMVECTOR& GetRightVector(bool omitY = false);
	const XMVECTOR& GetBackwardVector(bool omitY = false);
	const XMVECTOR& GetLeftVector(bool omitY = false);

protected:
	virtual void UpdateMatrix();
	void UpdateDirectionVectors();

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR _vec_forward;
	XMVECTOR _vec_left;
	XMVECTOR _vec_right;
	XMVECTOR _vec_backward;

	XMVECTOR _vec_forward_noY;
	XMVECTOR _vec_left_noY;
	XMVECTOR _vec_right_noY;
	XMVECTOR _vec_backward_noY;
};