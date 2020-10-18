#pragma once
#include <vector>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <D3D11.h>
#include "ConstantBufferTypes.h"
#include "ConstantBuffer.h"
#include "Texture.h"
using namespace std;
using namespace DirectX;
class Geometry
{
public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_color>& cb_vs_vertexshader_color);
	void Draw(const XMMATRIX& viewProjectionMatrix, const XMVECTOR& cameraPos);
private:
	VertexBuffer<ColorVertex> _vertexBuffer;
	IndexBuffer _indexBuffer;

	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader_color>* _cb_vs_vertexshader_color = nullptr;
	UINT _indexNum;
};

