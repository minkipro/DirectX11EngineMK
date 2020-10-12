#pragma once
#include <string>

using namespace std;
class StringHelper
{
public:
	static wstring StringToWide(string str);
	static string WideToString(wstring wstr);

	static std::string GetDirectoryFromPath(const std::string& filepath);
	static std::string GetFileExtension(const std::string& filename);
};