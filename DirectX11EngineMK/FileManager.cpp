#include "FileManager.h"
#include <filesystem>
#include <fstream>
typedef std::vector<std::string> stringvec;

struct path_leaf_string
{
	std::string operator()(const std::filesystem::directory_entry& entry) const
	{
		return entry.path().string();
	}
};

void read_directory(const std::string& name, stringvec& v)
{
	std::filesystem::path p(name);
	std::filesystem::directory_iterator start(p);
	std::filesystem::directory_iterator end;
	std::transform(start, end, std::back_inserter(v), path_leaf_string());
}


bool FileManager::GetTextData(std::vector<std::string>& vOut, std::string textFilePath)
{
	std::ifstream fin;
	fin.open(textFilePath);
	if (!fin.is_open())
		return false;

	char buffer[256] = { 0, };
	while (!fin.eof())
	{
		fin.getline(buffer, 256);
		vOut.push_back(buffer);
	}
	fin.close();
	return true;
}

void FileManager::GetFileList(std::vector<std::string>& vOut, std::string dir)
{
	read_directory(dir, vOut);
}
