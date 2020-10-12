#include "Shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), _shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderPath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreateVertexShader(_shaderBuffer.Get()->GetBufferPointer(), _shaderBuffer->GetBufferSize(), NULL, _shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create vertex shader: ";
		errorMsg += shaderPath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}
	
	hr = device->CreateInputLayout(layoutDesc, numElements, _shaderBuffer->GetBufferPointer(), _shaderBuffer->GetBufferSize(), _inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create input layout.");
		return false;
	}
	

	return true;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return _shader.Get();
}

ID3D10Blob* VertexShader::GetBuffer()
{
	return _shaderBuffer.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return _inputLayout.Get();
}

bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), _shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderPath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	hr = device->CreatePixelShader(_shaderBuffer.Get()->GetBufferPointer(), _shaderBuffer->GetBufferSize(), NULL, _shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create pixel shader: ";
		errorMsg += shaderPath;
		ErrorLogger::Log(hr, errorMsg);
		return false;
	}

	return true;
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return _shader.Get();
}

ID3D10Blob* PixelShader::GetBuffer()
{
	return _shaderBuffer.Get();
}
