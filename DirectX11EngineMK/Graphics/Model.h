#pragma once
#include "Mesh.h"
#include <unordered_map>
#include "..//Timer.h"

using namespace DirectX;

//static XMMATRIX openGlToDirectX = XMMATRIX(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
struct Bone
{
	int id = 0;
	std::string name = "";
	XMMATRIX offset = XMMatrixIdentity();
	std::vector<Bone> children = {};
};

struct BoneTransformTrack {
	std::vector<float> positionTimestamps = {};
	std::vector<float> rotationTimestamps = {};
	std::vector<float> scaleTimestamps = {};

	std::vector<aiVector3D> positions = {};
	std::vector<aiQuaternion> rotations = {};
	std::vector<aiVector3D> scales = {};
};

struct Animation {
	float duration = 0.0f;
	float ticksPerSecond = 1.0f;
	std::unordered_map<std::string, BoneTransformTrack> boneTransforms = {};
};

class Model
{
public:
	bool Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton);
	void Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix);

private:
	bool LoadModel(const std::string& filePath);
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix);
	TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);

	bool readSkeleton(Bone& boneOutput, aiNode* node, std::unordered_map<std::string, std::pair<int, XMMATRIX>>& boneInfoTable);
	//XMMATRIX assimpToXMMATRIX(aiMatrix4x4& mat);
	void loadAnimation(const aiScene* scene);

	std::pair<UINT, float> getTimeFraction(std::vector<float>& times, float& dt);
	void getPose(Animation& animation, Bone& skeletion, float dt, std::vector<XMMATRIX>& output, XMMATRIX& parentTransform, XMMATRIX& globalInverseTransform);
	aiVector3D Lerp(aiVector3D value1, aiVector3D value2, float amount);
	aiQuaternion Lerp(aiQuaternion value1, aiQuaternion value2, float amount);

private:
	std::vector<Mesh>					_meshes;

	ID3D11Device*						_device					= nullptr;
	ID3D11DeviceContext*				_deviceContext
		= nullptr;
	ConstantBuffer<CB_VS_vertexshader_skeleton>* _cb_vs_vertexshader_skeleton		= nullptr;
	std::string							_directory				= "";
	XMMATRIX _globalInverseTransform;
	Bone _skeletons;
	vector<Animation> _animation;
	Timer _timer;
	std::vector<XMMATRIX> _currentPose = {};
	vector<UINT> _boneCount;
};