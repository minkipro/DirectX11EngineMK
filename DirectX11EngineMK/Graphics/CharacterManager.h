#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "ConstantBuffer.h"
class FileManager;
class RenderableGameObject;
class CharacterManager
{
public:
	~CharacterManager();
	void Initialize(FileManager* fileManager, 
		ID3D11Device* device, 
		ID3D11DeviceContext* deviceContext, 
		ConstantBuffer<CB_VS_vertexshader_skeleton>& cb_vs_vertexshader_skeleton);

	void Draw(const DirectX::XMMATRIX& viewProjectionMatrix, float* currentTime);
	
private:
	std::vector<std::string> _characterNameList;
	std::unordered_map<std::string, RenderableGameObject*> _characters;
};