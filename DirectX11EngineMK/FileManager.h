#pragma once
#include <vector>
#include <string>
class FileManager
{
public:
	static bool GetTextData(std::vector<std::string>& vOut, std::string textFilePath);
	static void GetFileList(std::vector<std::string>& vOut, std::string dir);
private:
	
};