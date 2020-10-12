#pragma once

#include "GameObject3D.h"

class RenderableGameObject : public GameObject3D
{
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton);

	void Draw(const XMMATRIX& viewProjectionMatrix);

protected:
	Model _model;
	void UpdateMatrix() override;
	XMMATRIX							_worldMatrix = XMMatrixIdentity();
};