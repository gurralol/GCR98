#pragma once
#include <iostream>

#include "../utilities/filemanager.h"

class ControllerFileManager
{
public:
	ControllerFileManager();
	~ControllerFileManager();

	FileManager* fileManager;

	void OpenDirectory(std::filesystem::path directory);
	std::vector<std::filesystem::path> GetFiles();
};