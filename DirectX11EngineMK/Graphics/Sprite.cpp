#include "Sprite.h"
#include <WICTextureLoader.h>

bool Sprite::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float width, float height, std::string spritePath, ConstantBuffer<CB_VS_vertexshader_2D>& cb_vs_vertexshader_2d)
{
	_deviceContext = deviceContext;
	if (_deviceContext == nullptr)
		return false;

	_cb_vs_vertexshader_2d = &cb_vs_vertexshader_2d;

	_texture = std::make_unique<Texture>(device, spritePath, aiTextureType::aiTextureType_DIFFUSE);

	std::vector<Vertex2D> vertexData =
	{
		Vertex2D(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f), //TopLeft
		Vertex2D(0.5f, -0.5f, 0.0f, 1.0f, 0.0f), //TopRight
		Vertex2D(-0.5, 0.5, 0.0f, 0.0f, 1.0f), //Bottom Left
		Vertex2D(0.5, 0.5, 0.0f, 1.0f, 1.0f), //Bottom Right
	};

	std::vector<DWORD> indexData =
	{
		0, 1, 2,
		2, 1, 3
	};

	HRESULT hr = _vertices.Initialize(device, vertexData.data(), vertexData.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for sprite.");

	hr = _indices.Initialize(device, indexData.data(), indexData.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");

	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);

	SetScale(width, height);

	return true;
}

void Sprite::Draw(XMMATRIX orthoMatrix)
{
	XMMATRIX wvpMatrix = _worldMatrix * orthoMatrix;
	_deviceContext->VSSetConstantBuffers(0, 1, _cb_vs_vertexshader_2d->GetAddressOf());
	_cb_vs_vertexshader_2d->_data.wvpMatrix = wvpMatrix;
	_cb_vs_vertexshader_2d->ApplyChanges();

	_deviceContext->PSSetShaderResources(0, 1, _texture->GetTextureResourceViewAddress());

	const UINT offsets = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertices.GetAddressOf(), _vertices.StridePtr(), &offsets);
	_deviceContext->IASetIndexBuffer(_indices.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->DrawIndexed(_indices.IndexCount(), 0, 0);
}

float Sprite::GetWidth()
{
	return _scale.x;
}

float Sprite::GetHeight()
{
	return _scale.y;
}

void Sprite::UpdateMatrix()
{
	_worldMatrix = XMMatrixScaling(_scale.x, _scale.y, 1.0f) * XMMatrixRotationRollPitchYaw(_rot.x, _rot.y, _rot.z) * XMMatrixTranslation(_pos.x + _scale.x / 2.0f, _pos.y + _scale.y / 2.0f, _pos.z);
}