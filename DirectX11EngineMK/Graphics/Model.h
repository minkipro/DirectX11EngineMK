#pragma once
#include "Mesh.h"
#include <unordered_map>

using namespace DirectX;
struct BoneInfo
{
	XMMATRIX boneOffset;
	XMMATRIX finalTransform;
};

struct NodeData
{
	std::string name;
	XMMATRIX transfomation;
	std::vector<NodeData> childs;
};

struct KeyVectorData
{
	float time;
	aiVector3D vectorData;
};

struct KeyQuaterData
{
	float time;
	aiQuaternion quaterData;
};

struct NodeAnimData
{
	vector<KeyVectorData> scalingKeys;
	vector<KeyQuaterData> rotationKeys;
	vector<KeyVectorData> positionKeys;
};
struct AnimData
{
	float tickPerSecond;
	float duration;
	unordered_map<string, NodeAnimData> nodeAnimDatas;
};

class Model
{
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix, float* currentTime);
	bool GetIsAnim();
	bool LoadModel(const std::string& filePath);
private:
	void ProcessNode(const aiScene* scene, aiNode* node, const XMMATRIX& parentTransformMatrix, NodeData* data);
	Mesh ProcessMesh(const aiScene* scene, aiMesh* mesh, const XMMATRIX& transformMatrix);

	//Animation
	void SetAnimBoneTransform(float animationTime);
	void ProcessNodeAnim(float tick, NodeData* animData, const XMMATRIX& parentTransform);
	void CalNodeTransformMatrix(NodeAnimData* nodeAnim, XMMATRIX& nodeTransform, float tick);
	unsigned int FindKeyIndex(const float animationTime, vector<KeyVectorData>& vectorDatas);
	unsigned int FindKeyIndex(const float animationTime, vector<KeyQuaterData>& quaterDatas);
	void CalcInterpolatedValueFromKey(float animationTime, NodeAnimData* nodeAnimData, aiVector3D& scaling, aiQuaternion& rotationQ, aiVector3D& translation);
	
	//Texture
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);
	
	
	
	
private:
	std::vector<Mesh>					_meshes;

	ID3D11Device*						_device					= nullptr;
	ID3D11DeviceContext*				_deviceContext
		= nullptr;
	ConstantBuffer<CB_VS_vertexshader_skeleton>* _cb_vs_vertexshader_skeleton		= nullptr;
	std::string							_directory				= "";

	XMMATRIX _currentBone[100];
	std::unordered_map<std::string, std::pair<int, XMMATRIX>> _boneInfo = {};
	XMMATRIX _axisMatrix;
	std::vector<AnimData> _animations;
	size_t _animIndex;
	NodeData _rootNode;
};