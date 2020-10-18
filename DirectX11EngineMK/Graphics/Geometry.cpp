#include "Geometry.h"

bool Geometry::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_color>& cb_vs_vertexshader_color)
{
	_device = device;
	_deviceContext = deviceContext;
	_cb_vs_vertexshader_color = &cb_vs_vertexshader_color;
	
	int gridNum = 1000;
	vector<ColorVertex> vertices;
	UINT vertexNum = (4 * (2 * gridNum + 1))*(2*gridNum+1) + 2;
	vertices.reserve(vertexNum);
	for (int j = -gridNum; j <= gridNum; j+=10)
	{
		if (j == 0)
		{
			for (int k = -gridNum; k <= gridNum; k += 10)
			{
				vertices.push_back(ColorVertex(k, 0.0f, j, 1.0f, 0.0f, 0.0f, 1.0f));
				vertices.push_back(ColorVertex(k+10, 0.0f, j, 1.0f, 0.0f, 0.0f, 1.0f));
			}
		}
		else
		{
			for (int k = -gridNum; k <= gridNum; k += 10)
			{
				vertices.push_back(ColorVertex(k, 0.0f, j, 0.0f, 0.0f, 0.0f, 1.0f));
				vertices.push_back(ColorVertex(k+10, 0.0f, j, 0.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}

	for (int j = -gridNum; j <= gridNum; j+=10)
	{
		if (j == 0)
		{
			for (int k = -gridNum; k <= gridNum; k += 10)
			{
				vertices.push_back(ColorVertex(j, 0.0f, k, 1.0f, 0.0f, 0.0f, 1.0f));
				vertices.push_back(ColorVertex(j, 0.0f, k+10, 1.0f, 0.0f, 0.0f, 1.0f));
			}
		}
		else
		{
			for (int k = -gridNum; k <= gridNum; k += 10)
			{
				vertices.push_back(ColorVertex(j, 0.0f, k, 0.0f, 0.0f, 0.0f, 1.0f));
				vertices.push_back(ColorVertex(j, 0.0f, k+10, 0.0f, 0.0f, 0.0f, 1.0f));
			}
		}
	}
	vertices.push_back(ColorVertex(0.0f, -gridNum, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f));
	vertices.push_back(ColorVertex(0.0f, gridNum, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f));

	_vertexBuffer.Initialize(_device, vertices.data(), vertices.size());
	vector<DWORD> indices;
	indices.resize(vertices.size());
	for (int i = 0; i < vertices.size(); i++)
	{
		indices[i] = i;
	}
	_indexBuffer.Initialize(_device, &indices[0], indices.size());
	_indexNum = indices.size();
	return true;
}

void Geometry::Draw(const XMMATRIX& viewProjectionMatrix, const XMVECTOR& cameraPos)
{

	UINT offset = 0;
	_cb_vs_vertexshader_color->_data.wvpMatrix = viewProjectionMatrix;
	_cb_vs_vertexshader_color->_data.cameraPos = cameraPos;
	_cb_vs_vertexshader_color->ApplyChanges();
	_deviceContext->VSSetConstantBuffers(0, 1, _cb_vs_vertexshader_color->GetAddressOf());
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), _vertexBuffer.StridePtr(), &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->DrawIndexed(_indexNum, 0, 0);
}
