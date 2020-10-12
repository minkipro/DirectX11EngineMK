#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"

class Mesh
{
public:
	Mesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<Vertex3D_Skeleton>& vertices, std::vector<DWORD>& indices, std::vector<Texture>& textures, const DirectX::XMMATRIX& transformMatrix);
	Mesh(const Mesh& mesh);
	void Draw();
	const DirectX::XMMATRIX& GetTransformMatirx();
private:
	VertexBuffer<Vertex3D_Skeleton>	_vertexbuffer;
	IndexBuffer				_indexbuffer;
	ID3D11DeviceContext*	_deviceContext;
	std::vector<Texture>	_textures;
	DirectX::XMMATRIX		_transformMatrix;
};