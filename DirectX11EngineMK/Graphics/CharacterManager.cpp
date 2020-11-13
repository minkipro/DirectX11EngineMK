#include "CharacterManager.h"

#include "../FileManager.h"
#include "RenderableGameObject.h"

#include <d3d11.h>


CharacterManager::~CharacterManager()
{
	for (auto it : _characters)
	{
		if (nullptr != it.second)
		{
			delete it.second;
			it.second = nullptr;
		}
	}
}

void CharacterManager::Initialize(FileManager* fileManager, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton)
{
	fileManager->GetTextData(_characterNameList, "Data\\Objects\\characterList.txt");

	std::vector<std::string> fileList;
	fileManager->GetFileList(fileList, "Data\\Objects\\characters");
	
	size_t fileNum = fileList.size();
	size_t characterNum = _characterNameList.size();
	for (size_t i = 0; i < fileNum; i++)
	{
		for (size_t j = 0; j < characterNum; j++)
		{
			size_t strIndex = fileList[i].find(_characterNameList[j]);
			if (strIndex != std::string::npos)
			{
				auto curCharacter = _characters.find(_characterNameList[j]);
				if (curCharacter == _characters.end())
				{
					_characters[_characterNameList[j]] = new RenderableGameObject;
					_characters[_characterNameList[j]]->Initialize(fileList[i], device, deviceContext, cb_vs_vertexshader_skeleton);
				}
				else
				{
					_characters[_characterNameList[j]]->AddAnimation(fileList[i]);
				}
			}
		}
	}
	
}

void CharacterManager::Draw(const XMMATRIX& viewProjectionMatrix, float* currentTime)
{
	for (auto it : _characters)
	{
		if (it.second->GetIsAnim())
		{
			it.second->Draw(viewProjectionMatrix, currentTime);
		}
		else
		{
			it.second->Draw(viewProjectionMatrix, nullptr);
		}
	}
}


