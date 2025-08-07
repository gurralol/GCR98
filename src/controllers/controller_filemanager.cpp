#include "controller_filemanager.h"

ControllerFileManager::ControllerFileManager()
{
	fileManager = new FileManager();
}

ControllerFileManager::~ControllerFileManager()
{
}

void ControllerFileManager::OpenDirectory(std::filesystem::path directory)
{
	fileManager->directory = directory;
	fileManager->OpenDirectory();
}

std::vector<std::filesystem::path> ControllerFileManager::GetFiles()
{
	return fileManager->filePaths;
}
