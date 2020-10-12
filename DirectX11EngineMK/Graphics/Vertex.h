#pragma once
#include <DirectXMath.h>

struct Vertex2D
{
	Vertex2D() : _pos(0.0f, 0.0f, 0.0f), _texCoord(0.0f, 0.0f) {}
	Vertex2D(float x, float y, float z, float u, float v)
		:_pos(x, y, z), _texCoord(u, v) {}

	DirectX::XMFLOAT3 _pos;
	DirectX::XMFLOAT2 _texCoord;
};

struct Vertex3D
{
	Vertex3D() : _pos(0.0f, 0.0f, 0.0f), _texCoord(0.0f, 0.0f), _normal(1.0f, 0.0f, 0.0f){}
	Vertex3D(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		:_pos(x, y, z), _texCoord(u, v), _normal(nx, ny, nz) {}

	DirectX::XMFLOAT3 _pos;
	DirectX::XMFLOAT2 _texCoord;
	DirectX::XMFLOAT3 _normal;
};

struct Vertex3D_Skeleton
{
	Vertex3D_Skeleton() : _pos(0.0f, 0.0f, 0.0f), _texCoord(0.0f, 0.0f), _normal(1.0f, 0.0f, 0.0f), _boneIds1(0.0f, 0.0f, 0.0f, 0.0f), 
		_boneIds2(0.0f, 0.0f, 0.0f, 0.0f), _boneWeights1(0.0f, 0.0f, 0.0f, 0.0f), _boneWeights2(0.0f, 0.0f, 0.0f, 0.0f) {}
	Vertex3D_Skeleton(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		:_pos(x, y, z), _texCoord(u, v), _normal(nx, ny, nz),
		_boneIds1(0.0f, 0.0f, 0.0f, 0.0f),
		_boneIds2(0.0f, 0.0f, 0.0f, 0.0f), _boneWeights1(0.0f, 0.0f, 0.0f, 0.0f), _boneWeights2(0.0f, 0.0f, 0.0f, 0.0f) {}

	DirectX::XMFLOAT3 _pos;
	DirectX::XMFLOAT2 _texCoord;
	DirectX::XMFLOAT3 _normal;
	DirectX::XMFLOAT4 _boneIds1;
	DirectX::XMFLOAT4 _boneIds2;
	DirectX::XMFLOAT4 _boneWeights1;
	DirectX::XMFLOAT4 _boneWeights2;
};

struct ColorVertex
{
	ColorVertex(float x, float y, float z, float r, float g, float b, float a)
		:_pos(x, y, z), _color(r, g, b, a) {}
	DirectX::XMFLOAT3 _pos;
	DirectX::XMFLOAT4 _color;
};

