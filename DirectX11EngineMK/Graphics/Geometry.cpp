#include "Geometry.h"

bool Geometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_color>& cb_vs_vertexshader_color)
{
	_device = device;
	_deviceContext = deviceContext;
	_cb_vs_vertexshader_color = &cb_vs_vertexshader_color;
	vector<ColorVertex> vertices;
	for (int j = 0; j <= 20; j++)
	{
		vertices.push_back(ColorVertex(-100.0f, 0.0f, -100.0f + j * 10, 0.0f, 0.0f, 0.0f, 1.0f));
		vertices.push_back(ColorVertex(100.0f, 0.0f, -100.0f + j * 10, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	for (int j = 0; j <= 20; j++)
	{
		vertices.push_back(ColorVertex(-100.0f+j*10, 0.0f, -100.0f, 0.0f, 0.0f, 0.0f, 1.0f));
		vertices.push_back(ColorVertex(-100.0f+j * 10, 0.0f, 100.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	}

	_vertexBuffer.Initialize(_device, vertices.data(), vertices.size());
	DWORD indices[100];
	for (int i = 0; i < vertices.size(); i++)
	{
		indices[i] = i;
	}
	_indexBuffer.Initialize(_device, indices, 100);
	return true;
}

void Geometry::Draw(const XMMATRIX& viewProjectionMatrix)
{

	UINT offset = 0;
	_deviceContext->VSSetConstantBuffers(0, 1, _cb_vs_vertexshader_color->GetAddressOf());
	_cb_vs_vertexshader_color->_data.wvpMatrix = viewProjectionMatrix;
	_cb_vs_vertexshader_color->ApplyChanges();
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), _vertexBuffer.StridePtr(), &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->DrawIndexed(100, 0, 0);
}
