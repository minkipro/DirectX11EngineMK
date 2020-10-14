#include "Model.h"


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
	_timer.Start();
	return true;
}

void Model::Draw(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix)
{
	XMMATRIX parentTransform = XMMatrixIdentity();
	getPose(_animation[0], _skeletons, _timer.GetMilisecondsElapsed(), _currentPose, parentTransform, _globalInverseTransform);
	_deviceContext->VSSetConstantBuffers(0, 1, _cb_vs_vertexshader_skeleton->GetAddressOf());
	for (int i = 0; i < _meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		_cb_vs_vertexshader_skeleton->_data.worldMatrix = _meshes[i].GetTransformMatirx() * worldMatrix;
		_cb_vs_vertexshader_skeleton->_data.wvpMatrix = _cb_vs_vertexshader_skeleton->_data.worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
		for (size_t i = 0; i < _currentPose.size(); i++)
		{
			_cb_vs_vertexshader_skeleton->_data.boneTransform[i] = _currentPose[i];
		}

		_cb_vs_vertexshader_skeleton->ApplyChanges();
		_meshes[i].Draw();
	}
}

bool Model::LoadModel(const std::string& filePath)
{

	_directory = StringHelper::GetDirectoryFromPath(filePath);
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr)
		return false;

	_globalInverseTransform = XMMATRIX(&pScene->mRootNode->mTransformation.Inverse().a1);

	ProcessNode(pScene->mRootNode, pScene, DirectX::XMMatrixIdentity());
	loadAnimation(pScene);
	_currentPose.resize(_boneCount[0], XMMatrixIdentity());

	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix)
{
	XMMATRIX nodeTransformMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(ProcessMesh(mesh, scene, nodeTransformMatrix));
	}
	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, nodeTransformMatrix);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix)
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

	std::unordered_map<std::string, std::pair<int, XMMATRIX>> boneInfo = {};
	std::vector<UINT> boneCounts;
	_boneCount.push_back(mesh->mNumBones);
	boneCounts.resize(vertices.size(), 0);

	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* bone = mesh->mBones[i];
		XMMATRIX offsetMatrix = XMMatrixTranspose(XMMATRIX(&bone->mOffsetMatrix.a1));
		//XMMATRIX offsetMatrix = XMMATRIX(&bone->mOffsetMatrix.a1)*openGlToDirectX;
		boneInfo[bone->mName.C_Str()] = { i, offsetMatrix };

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
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	readSkeleton(_skeletons, scene->mRootNode, boneInfo);

	return Mesh(_device, _deviceContext, vertices, indices, textures, transformMatrix);
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

bool Model::readSkeleton(Bone& boneOutput, aiNode* node, std::unordered_map<std::string, std::pair<int, XMMATRIX>>& boneInfoTable)
{
	if (boneInfoTable.find(node->mName.C_Str()) != boneInfoTable.end())
	{
		boneOutput.name = node->mName.C_Str();
		boneOutput.id = boneInfoTable[boneOutput.name].first;
		boneOutput.offset = boneInfoTable[boneOutput.name].second;

		for (UINT i = 0; i < node->mNumChildren; i++)
		{
			Bone child;
			readSkeleton(child, node->mChildren[i], boneInfoTable);
			boneOutput.children.push_back(child);
		}
		return true;
	}
	else
	{
		for (int i = 0; i < node->mNumChildren; i++)
		{
			if (readSkeleton(boneOutput, node->mChildren[i], boneInfoTable))
			{
				return true;
			}
		}
	}
	return false;
}

void Model::loadAnimation(const aiScene* scene)
{
	for (UINT k = 0; k < scene->mNumAnimations; k++)
	{
		_animation.push_back(Animation());
		Animation& animation = _animation.back();
		aiAnimation* anim = scene->mAnimations[k];

		if (anim->mTicksPerSecond != 0.0f)
			animation.ticksPerSecond = anim->mTicksPerSecond;
		else
			animation.ticksPerSecond = 1;


		animation.duration = anim->mDuration * anim->mTicksPerSecond;
		animation.boneTransforms = {};

		//load positions rotations and scales for each bone
		// each channel represents each bone
		for (int i = 0; i < anim->mNumChannels; i++) {
			aiNodeAnim* channel = anim->mChannels[i];
			BoneTransformTrack track;
			for (int j = 0; j < channel->mNumPositionKeys; j++) {
				track.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
				track.positions.push_back(channel->mPositionKeys[j].mValue);
			}
			for (int j = 0; j < channel->mNumRotationKeys; j++) {
				track.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
				track.rotations.push_back(channel->mRotationKeys[j].mValue);

			}
			for (int j = 0; j < channel->mNumScalingKeys; j++) {
				track.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
				track.scales.push_back(channel->mScalingKeys[j].mValue);

			}
			animation.boneTransforms[channel->mNodeName.C_Str()] = track;
		}
	}
}

std::pair<UINT, float> Model::getTimeFraction(std::vector<float>& times, float& dt)
{
	if (times.size() <= 1)
		return { 0, 0.0f };
	UINT segment = 0;
	while (dt > times[segment])
		segment++;
	float start = times[segment - 1];
	float end = times[segment];
	float frac = (dt - start) / (end - start);
	return { segment, frac };
}

void Model::getPose(Animation& animation, Bone& skeletion, float dt, std::vector<XMMATRIX>& output, XMMATRIX& parentTransform, XMMATRIX& globalInverseTransform)
{
	BoneTransformTrack& btt = animation.boneTransforms[skeletion.name];

	dt = fmod(dt, animation.duration);

	float newdt = dt / animation.duration * (btt.positionTimestamps.size() - 1);
	if (newdt > 10.0f)
	{
		int a = 1;
	}
	std::pair<UINT, float> fp;

	aiVector3D position = aiVector3D(0.0f, 0.0f, 0.0f);
	aiQuaternion rotation = aiQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
	aiVector3D scale = aiVector3D(1.0f, 1.0f, 1.0f);
	//calculate interpolated position
	if (btt.positionTimestamps.size() > 1)
	{
		fp = getTimeFraction(btt.positionTimestamps, newdt);

		aiVector3D position1 = btt.positions[fp.first - 1];
		aiVector3D position2 = btt.positions[fp.first];
		position = this->Lerp(position1, position2, fp.second);
	}
	else if (btt.positionTimestamps.size() == 1)
	{
		position = btt.positions[0];
	}
	//calculate interpolated rotation

	if (btt.positionTimestamps.size() > 1)
	{
		fp = getTimeFraction(btt.rotationTimestamps, newdt);
		aiQuaternion rotation1 = btt.rotations[fp.first - 1].Normalize();
		aiQuaternion rotation2 = btt.rotations[fp.first].Normalize();
		aiQuaternion::Interpolate(rotation, rotation1, rotation2, fp.second);
	}
	else if (btt.positionTimestamps.size() == 1)
	{
		rotation = btt.rotations[0];
	}

	//calculate interpolated scale
	if (btt.positionTimestamps.size() > 1)
	{
		fp = getTimeFraction(btt.scaleTimestamps, newdt);
		aiVector3D scale1 = btt.scales[fp.first - 1];
		aiVector3D scale2 = btt.scales[fp.first];

		scale = this->Lerp(scale1, scale2, fp.second);
	}
	else if (btt.positionTimestamps.size() == 1)
	{
		scale = btt.scales[0];
	}
	XMMATRIX positionMat = XMMatrixIdentity(),
		scaleMat = XMMatrixIdentity();
	// calculate localTransform
	XMVECTOR rotationQuater = {rotation.x, rotation.y, rotation.z, rotation.w};
	XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuater);


	positionMat = XMMatrixTranslation(position.x, position.y, position.z);
	scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX localTransform = scaleMat * rotationMatrix * positionMat;
	aiMatrix4x4 ailocalTransform = aiMatrix4x4(scale, rotation, position);
	//XMMATRIX localTransform = XMMATRIX(&ailocalTransform.a1);
	XMMATRIX globalTransform = localTransform * parentTransform;
	if (animation.boneTransforms.find(skeletion.name) != animation.boneTransforms.end())
	{
		output[skeletion.id] = (skeletion.offset * globalTransform)*globalInverseTransform;
	}
	
	//output[skeletion.id] = localTransform;
	//update values for children bones
	for (Bone& child : skeletion.children) {
		getPose(animation, child, dt, output, globalTransform, globalInverseTransform);
	}
}

aiVector3D Model::Lerp(aiVector3D value1, aiVector3D value2, float amount)
{
	if (amount < 0.0f || amount>1.0f)
	{
		int a = 1;
	}
	return value1 + ((value2 - value1) * amount);
}

//XMMATRIX Model::assimpToXMMATRIX(aiMatrix4x4& mat)
//{
//	DirectX::XMMATRIX m;
//	m.r[0]_
//	for (int y = 0; y < 4; y++)
//	{
//		for (int x = 0; x < 4; x++)
//		{
//			m[0][0];
//			m[0][0] = mat[y][x];
//		}
//	}
//	return m;
//}
