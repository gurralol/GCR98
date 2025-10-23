#pragma once
#include <wx/wxprec.h>
#include "../gcr_menubar_base.h"

enum
{
	gcr_OPEN_FILES,
	gcr_OPEN_DIRECTORY,
	gcr_EXIT = wxID_EXIT,

	gcr_FILTER_LEVELS,
	gcr_FILTER_BLUR,
	gcr_FILTER_FACESWAP,

	gcr_ABOUT = wxID_ABOUT,

	gcr_FILETHUMB,

	gcr_ADD_TO_PLAYLIST,
	gcr_PREFERENCES
};

class gcr_menubar_mainframe : public gcr_menubar_base
{
public:
	gcr_menubar_mainframe();
	~gcr_menubar_mainframe();

	wxMenu* menuFile;
	wxMenu* menuFileRecentFiles;
	wxMenu* menuFileRecentDirectories;

	wxMenu* menuView;

	wxMenu* menuFilters;
	wxMenu* menuFiltersBlur;
	wxMenu* menuFiltersEnhance;
	wxMenu* menuFiltersColorAdjustment;
	wxMenu* menuFiltersNoise;
	wxMenu* menuFiltersAi;
	wxMenu* menuFiltersSpecial;

	wxMenu* menuRender;

	wxMenu* menuSettings;

	wxMenu* menuHelp;
};