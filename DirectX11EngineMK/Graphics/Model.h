#pragma once
#include "Mesh.h"
#include <unordered_map>
#include "..//Timer.h"

using namespace DirectX;
struct BoneInfo
{
	XMMATRIX boneOffset;
	XMMATRIX finalTransform;
};

class Model
{
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);

private:
	bool LoadModel(const std::string& filePath);
	void ProcessNode(aiNode* node, const XMMATRIX& parentTransformMatrix);
	Mesh ProcessMesh(aiMesh* mesh, const XMMATRIX& transformMatrix);
	void SetAnimBoneTransform(float animationTime, const int animationIndex);
	void ProcessNodeAnim(float animationTime, aiNode* node, const XMMATRIX& parentTransform);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);
	aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const string nodeName);

private:
	std::vector<Mesh>					_meshes;

	ID3D11Device*						_device					= nullptr;
	ID3D11DeviceContext*				_deviceContext
		= nullptr;
	ConstantBuffer<CB_VS_vertexshader_skeleton>* _cb_vs_vertexshader_skeleton		= nullptr;
	std::string							_directory				= "";
	XMMATRIX _globalInverseTransform;
	Timer _timer;

	Assimp::Importer _importer;
	const aiScene* _pScene = nullptr;

	XMMATRIX _currentBone[100];

	std::unordered_map<std::string, std::pair<int, XMMATRIX>> _boneInfo = {};
};