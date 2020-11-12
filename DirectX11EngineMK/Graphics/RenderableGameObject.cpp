#include "RenderableGameObject.h"

bool RenderableGameObject::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton)
{
	if (!_model.Initialize(filePath, device, deviceContext, cb_vs_vertexshader_skeleton))
		return false;

	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	UpdateMatrix();
	return true;
}

void RenderableGameObject::Draw(const XMMATRIX& viewProjectionMatrix, float* currentTime)
{
	_model.Draw(_worldMatrix, viewProjectionMatrix, currentTime);
}

void RenderableGameObject::UpdateMatrix()
{
	_worldMatrix = XMMatrixRotationRollPitchYaw(_rot.x, _rot.y, _rot.z) * XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
	UpdateDirectionVectors();
}