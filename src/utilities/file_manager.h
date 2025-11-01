#pragma once

//----------------//
//  File Manager  // 
//----------------//
#include <filesystem>
#include <vector>

enum
{
	FM_SORT_NAME = 0,
	FM_SORT_DATE = 1,
	FM_SORT_SIZE = 2,
	FM_SORT_TYPE = 3,
};

enum
{
	FM_FILE_TYPE_UNKNOWN = -1,
	FM_FILE_TYPE_VIDEO = 0,
	FM_FILE_TYPE_WEBP = 1,
	FM_FILE_TYPE_IMAGE = 2,
	FM_FILE_TYPE_AUDIO = 3,
};

class file_manager
{
public:
	file_manager();
	~file_manager();
	
	std::filesystem::path m_directory;
	std::vector<std::filesystem::path> m_files;

	int set_directory(std::filesystem::path path);
	int open_directory(bool recursive);
	int sort_files(int sort_mode);
	int clear_files();

	int file_type(std::filesystem::path path);
};

//-------------------------//
//  File Manager Extended  // 
//-------------------------//
#include <wx/wxprec.h>

class file_manager_extended
{
public:
	file_manager_extended();
	~file_manager_extended();

	int sort_buttons(std::vector<wxButton*> arr_buttons, int sort_mode);
};
