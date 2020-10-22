#include "Model.h"

unsigned int FindKeyIndex(const float animationTime, const int numKeys, const aiVectorKey* const vectorKey)
{
	assert(numKeys > 0);
	for (int i = 0; i < numKeys - 1; i++)
		if (animationTime < (float)vectorKey[i + 1].mTime)
			return i;
	assert(0);

	return 0;
}

unsigned int FindKeyIndex(const float animationTime, const int numKeys, const aiQuatKey* const quatKey)
{
	assert(numKeys > 0);
	for (int i = 0; i < numKeys - 1; i++)
		if (animationTime < (float)quatKey[i + 1].mTime)
			return i;
	assert(0);

	return 0;
}

aiVector3D CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiVectorKey* const vectorKey)
{
	aiVector3D ret;
	if (numKeys == 1)
	{
		ret = vectorKey[0].mValue;
		return ret;
	}

	unsigned int keyIndex = FindKeyIndex(animationTime, numKeys, vectorKey);
	unsigned int nextKeyIndex = keyIndex + 1;

	assert(nextKeyIndex < numKeys);

	float deltaTime = vectorKey[nextKeyIndex].mTime - vectorKey[keyIndex].mTime;
	float factor = (animationTime - (float)vectorKey[keyIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& startValue = vectorKey[keyIndex].mValue;
	const aiVector3D& endValue = vectorKey[nextKeyIndex].mValue;

	ret.x = startValue.x + (endValue.x - startValue.x) * factor;
	ret.y = startValue.y + (endValue.y - startValue.y) * factor;
	ret.z = startValue.z + (endValue.z - startValue.z) * factor;

	return ret;
}

aiQuaternion CalcInterpolatedValueFromKey(float animationTime, const int numKeys, const aiQuatKey* const quatKey)
{
	aiQuaternion ret;
	if (numKeys == 1)
	{
		ret = quatKey[0].mValue;
		return ret;
	}

	unsigned int keyIndex = FindKeyIndex(animationTime, numKeys, quatKey);
	unsigned int nextKeyIndex = keyIndex + 1;

	assert(nextKeyIndex < numKeys);

	float deltaTime = quatKey[nextKeyIndex].mTime - quatKey[keyIndex].mTime;
	float factor = (animationTime - (float)quatKey[keyIndex].mTime) / deltaTime;

	assert(factor >= 0.0f && factor <= 1.0f);

	const aiQuaternion& startValue = quatKey[keyIndex].mValue;
	const aiQuaternion& endValue = quatKey[nextKeyIndex].mValue;
	aiQuaternion::Interpolate(ret, startValue, endValue, factor);
	ret = ret.Normalize();

	return ret;
}
bool Model::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton)
{
	_device = device;
	_deviceContext = deviceContext;
	_cb_vs_vertexshader_skeleton = &cb_vs_vertexshader_skeleton;
	for (int i = 0; i < 100; i++)
	{
		_cb_vs_vertexshader_skeleton->_data.boneTransform[i] = XMMatrixIdentity();
	}

	try
	{
		if (!LoadModel(filePath))
			return false;
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	for (int i = 0; i < 100; i++)
	{
		_currentBone[i] = XMMatrixIdentity();
	}

	_timer.Start();
	return true;
}

void Model::Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	float currentTime = (float)_timer.GetMilisecondsElapsed();
	SetAnimBoneTransform(currentTime, 0);
	_deviceContext->VSSetConstantBuffers(0, 1, _cb_vs_vertexshader_skeleton->GetAddressOf());
	for (int i = 0; i < _meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		_cb_vs_vertexshader_skeleton->_data.worldMatrix = _meshes[i].GetTransformMatirx() * worldMatrix;
		_cb_vs_vertexshader_skeleton->_data.wvpMatrix = _cb_vs_vertexshader_skeleton->_data.worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix

		for (int i = 0; i < 100; i++)
		{
			_cb_vs_vertexshader_skeleton->_data.boneTransform[i] = _currentBone[i];

		}
		_cb_vs_vertexshader_skeleton->ApplyChanges();
		_meshes[i].Draw();
	}
}


bool Model::LoadModel(const std::string& filePath)
{

	_directory = StringHelper::GetDirectoryFromPath(filePath);


	_pScene = _importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);
	if (_pScene == nullptr)
		return false;

	_globalInverseTransform = XMMATRIX(&_pScene->mRootNode->mTransformation.Inverse().a1);

	ProcessNode(_pScene->mRootNode, DirectX::XMMatrixIdentity());

	return true;
}

void Model::ProcessNode(aiNode* node, const XMMATRIX& parentTransformMatrix)
{
	XMMATRIX nodeTransformMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = _pScene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(ProcessMesh(mesh, nodeTransformMatrix));
	}
	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], nodeTransformMatrix);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const XMMATRIX& transformMatrix)
{
	std::vector<Vertex3D_Skeleton> vertices;
	std::vector<DWORD> indices;

	vertices.reserve(mesh->mNumVertices);
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D_Skeleton vertex;

		vertex._pos.x = mesh->mVertices[i].x;
		vertex._pos.y = mesh->mVertices[i].y;
		vertex._pos.z = mesh->mVertices[i].z;

		vertex._normal.x = mesh->mNormals[i].x;
		vertex._normal.y = mesh->mNormals[i].y;
		vertex._normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex._texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex._texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}


	std::vector<UINT> boneCounts;
	boneCounts.resize(vertices.size(), 0);

	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* bone = mesh->mBones[i];
		XMMATRIX offsetMatrix = XMMatrixTranspose(XMMATRIX(&bone->mOffsetMatrix.a1));
		//XMMATRIX offsetMatrix = XMMATRIX(&bone->mOffsetMatrix.a1)*openGlToDirectX;
		_boneInfo[bone->mName.C_Str()] = { i, offsetMatrix };

		for (UINT j = 0; j < bone->mNumWeights; j++)
		{
			UINT id = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			boneCounts[id]++;
			switch (boneCounts[id])
			{
			case 1:
				vertices[id]._boneIds1.x = i;
				vertices[id]._boneWeights1.x = weight;
				break;
			case 2:
				vertices[id]._boneIds1.y = i;
				vertices[id]._boneWeights1.y = weight;
				break;
			case 3:
				vertices[id]._boneIds1.z = i;
				vertices[id]._boneWeights1.z = weight;
				break;
			case 4:
				vertices[id]._boneIds1.w = i;
				vertices[id]._boneWeights1.w = weight;
				break;
			case 5:
				vertices[id]._boneIds2.x = i;
				vertices[id]._boneWeights2.x = weight;
				break;
			case 6:
				vertices[id]._boneIds2.y = i;
				vertices[id]._boneWeights2.y = weight;
				break;
			case 7:
				vertices[id]._boneIds2.z = i;
				vertices[id]._boneWeights2.z = weight;
				break;
			case 8:
				vertices[id]._boneIds2.w = i;
				vertices[id]._boneWeights2.w = weight;
				break;
			default:
			{
				int a = 1;

			}
			//std::cout << "err: unable to allocate bone to vertex" << std::endl;
			break;
			}
		}
	}

	for (size_t i = 0; i < vertices.size(); i++)
	{
		XMFLOAT4& boneWeights1 = vertices[i]._boneWeights1;
		XMFLOAT4& boneWeights2 = vertices[i]._boneWeights2;
		float totalWeight = boneWeights1.x + boneWeights1.y + boneWeights1.z + boneWeights1.w
			+ boneWeights2.x + boneWeights2.y + boneWeights2.z + boneWeights2.w;
		if (totalWeight > 0.0f)
		{
			vertices[i]._boneWeights1 = XMFLOAT4(
				boneWeights1.x / totalWeight,
				boneWeights1.y / totalWeight,
				boneWeights1.z / totalWeight,
				boneWeights1.w / totalWeight
			);

			vertices[i]._boneWeights2 = XMFLOAT4(
				boneWeights2.x / totalWeight,
				boneWeights2.y / totalWeight,
				boneWeights2.z / totalWeight,
				boneWeights2.w / totalWeight
			);
		}
	}

	indices.reserve(mesh->mNumFaces);
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	std::vector<Texture> textures;
	aiMaterial* material = _pScene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, _pScene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	return Mesh(_device, _deviceContext, vertices, indices, textures, transformMatrix);
}

void Model::SetAnimBoneTransform(float animationTime, const int animationIndex)
{
	animationTime /= _pScene->mAnimations[animationIndex]->mDuration;
	animationTime = fmod(animationTime, _pScene->mAnimations[animationIndex]->mDuration);
	ProcessNodeAnim(animationTime, _pScene->mRootNode, XMMatrixIdentity());
}

void Model::ProcessNodeAnim(float animationTime, aiNode* node, const XMMATRIX& parentTransform)
{
	string nodeName = node->mName.data;
	const aiAnimation* animation = _pScene->mAnimations[0];

	XMMATRIX nodeTransformMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1));

	const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);
	if (nodeAnim)
	{
		const aiVector3D& scaling = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumScalingKeys, nodeAnim->mScalingKeys);
		XMMATRIX scalingM = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
		
		const aiQuaternion& rotationQ = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumRotationKeys, nodeAnim->mRotationKeys);
		XMMATRIX rotationM = XMMatrixRotationQuaternion({ rotationQ.x, rotationQ.y, rotationQ.z, rotationQ.w });

		const aiVector3D& translation = CalcInterpolatedValueFromKey(animationTime, nodeAnim->mNumPositionKeys, nodeAnim->mPositionKeys);
		XMMATRIX translationM = XMMatrixTranslation(translation.x, translation.y, translation.z);
		nodeTransformMatrix = scalingM * rotationM * translationM;
	}

	XMMATRIX globalTransform = nodeTransformMatrix * parentTransform;
	if (_boneInfo.find(nodeName) != _boneInfo.end())
	{
		XMMATRIX finalTransform = _boneInfo[nodeName].second * globalTransform * XMMatrixTranspose(XMMATRIX(&_pScene->mRootNode->mTransformation.a1));
		_currentBone[_boneInfo[nodeName].first] = finalTransform;
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNodeAnim(animationTime, node->mChildren[i], globalTransform);
	}
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture exists
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) //If there are no textures
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				materialTextures.push_back(Texture(_device, Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(_device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storetype)
			{
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture(_device,
					reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
					pScene->mTextures[index]->mWidth,
					textureType);
				materialTextures.push_back(embeddedIndexedTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(_device,
					reinterpret_cast<uint8_t*>(pTexture->pcData),
					pTexture->mWidth,
					textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::Disk:
			{
				std::string filename = _directory + '\\' + path.C_Str();
				Texture diskTexture(_device, filename, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			}
		}
	}
	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(_device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;
}

int Model::GetTextureIndex(aiString* pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}

aiNodeAnim* Model::FindNodeAnim(const aiAnimation* animation, const string nodeName)
{
	for (int i = 0; i < animation->mNumChannels; i++)
		if (animation->mChannels[i]->mNodeName.data == nodeName)
			return animation->mChannels[i];
	return nullptr;
}
