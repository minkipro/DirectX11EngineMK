#pragma once
#include <d3d11.h>
#include "ConstantBufferTypes.h"
#include <wrl/client.h>
#include "..\\ErrorLogger.h"

template<class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T>& rhs);

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer>	_buffer;
	ID3D11DeviceContext*					_deviceContext = nullptr;

public:
	T _data;

public:
	ConstantBuffer() {}

	ID3D11Buffer* Get()const
	{
		return _buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return _buffer.GetAddressOf();
	}

	HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		if (_buffer.Get() != nullptr)
			_buffer.Reset();

		_deviceContext = deviceContext;

		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
		desc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&desc, 0, _buffer.GetAddressOf());
		return hr;
	}

	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = _deviceContext->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to map constant buffer.");
			return false;
		}
		CopyMemory(mappedResource.pData, &_data, sizeof(T));
		_deviceContext->Unmap(_buffer.Get(), 0);
		return true;
	}
};