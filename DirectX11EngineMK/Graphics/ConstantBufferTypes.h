#pragma once
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

struct CB_VS_vertexshader_color
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMVECTOR cameraPos;
};

struct CB_VS_vertexshader_2D
{
	DirectX::XMMATRIX wvpMatrix;
};

struct CB_PS_light
{
	DirectX::XMFLOAT3 ambientLightColor;
	float amblentLightStrength;

	DirectX::XMFLOAT3 dynamicLightColor;
	float dynamicLightStrength;

	DirectX::XMFLOAT3 dynamicLightPosition;
};

struct CB_VS_vertexshader_skeleton
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMMATRIX boneTransform[100];
};