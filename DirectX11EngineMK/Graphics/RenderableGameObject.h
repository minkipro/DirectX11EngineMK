#pragma once

#include "GameObject3D.h"

class RenderableGameObject : public GameObject3D
{
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton);
	void AddAnimation(const std::string& filePath);
	void Draw(const XMMATRIX& viewProjectionMatrix, float* currentTime);
	Model* GetModel() { return &_model; }
	XMMATRIX* GetWorldMatrix() { return &_worldMatrix; }
	bool GetIsAnim() { return _model.GetIsAnim(); }

protected:
public:
	Model _model;
	void UpdateMatrix() override;
	XMMATRIX							_worldMatrix = XMMatrixIdentity();
};