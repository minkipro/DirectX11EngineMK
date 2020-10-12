#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
	UINT _stride = sizeof(T);
	UINT _vertexCount = 0;

public:
	VertexBuffer() {}

	VertexBuffer(const VertexBuffer<T>& rhs)
	{
		_buffer == rhs._buffer;
		_vertexCount = rhs._vertexCount;
		_stride = rhs._stride;
	}

	VertexBuffer<T>& operator=(const VertexBuffer<T>& a)
	{
		_buffer = a._buffer;
		_vertexCount = a._vertexCount;
		_stride = a._stride;
		return *this;
	}

	ID3D11Buffer* Get()const
	{
		return _buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return _buffer.GetAddressOf();
	}

	UINT VertexCount() const
	{
		return _vertexCount;
	}

	const UINT Stride() const
	{
		return _stride;
	}

	const UINT* StridePtr() const
	{
		return &_stride;
	}

	HRESULT Initialize(ID3D11Device* device, T* data, UINT vertexCount)
	{
		if (_buffer.Get() != nullptr)
			_buffer.Reset();

		_vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = _stride * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, _buffer.GetAddressOf());
		return hr;
	}
};