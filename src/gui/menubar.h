#pragma once
#include <wx/wxprec.h>

class MenuBar : public wxMenuBar
{
public:
	MenuBar();
	~MenuBar();

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