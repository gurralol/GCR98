#pragma once

//----------------//
//  File Manager  // 
//----------------//
#include <filesystem>
#include <vector>

class FileManager
{
public:
	FileManager();
	~FileManager();

	std::filesystem::path directory;
	std::vector<std::filesystem::path> filePaths;

	int recursiveMode = 0;
	int sortMode = 0;

	enum //SORT
	{
		SORT_NAME,
		SORT_DATE,
		SORT_SIZE,
		SORT_TYPE,
	};

	void OpenDirectory();
	void Sort();

	bool IsSupportedFile(std::filesystem::path path);
	int IsImageFile(std::filesystem::path path);
};

//-------------------------//
//  File Manager Extended  // 
//-------------------------//
#include <wx/wxprec.h>

class FileManagerExtended
{
public:
	FileManagerExtended();
	~FileManagerExtended();

	int sortMode = 0;

	enum //SORT
	{
		SORT_NAME,
		SORT_DATE,
		SORT_SIZE,
		SORT_TYPE,
	};

	void SortButtons(std::vector<wxButton*> arrButtons);
};
