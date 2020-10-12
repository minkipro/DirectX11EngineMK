#pragma once
#include "GameObject2D.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include <string>
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Vertex.h"

class Sprite : public GameObject2D
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float width, float height, std::string spritePath, ConstantBuffer<CB_VS_vertexshader_2D>& cb_vs_vertexshader_2d);
	void Draw(XMMATRIX orthoMatrix); //2d camera orthogonal matrix
	float GetWidth();
	float GetHeight();
private:
	void UpdateMatrix() override;

	ConstantBuffer<CB_VS_vertexshader_2D>*	_cb_vs_vertexshader_2d = nullptr;
	XMMATRIX								_worldMatrix = XMMatrixIdentity();

	std::unique_ptr<Texture>				_texture;
	ID3D11DeviceContext*					_deviceContext = nullptr;

	IndexBuffer								_indices;
	VertexBuffer<Vertex2D>					_vertices;
};