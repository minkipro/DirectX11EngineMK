#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
	UINT _indexCount = 0;
public:
	IndexBuffer() {}

	ID3D11Buffer* Get()const
	{
		return _buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return _buffer.GetAddressOf();
	}

	UINT IndexCount() const
	{
		return _indexCount;
	}

	HRESULT Initialize(ID3D11Device* device, DWORD* data, UINT indexCount)
	{
		if (_buffer.Get() != nullptr)
			_buffer.Reset();

		_indexCount = indexCount;
		//Load Index Data
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;
		HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferData, _buffer.GetAddressOf());
		return hr;
	}
};