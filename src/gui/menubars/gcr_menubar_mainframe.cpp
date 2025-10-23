#include "gcr_menubar_mainframe.h"

gcr_menubar_mainframe::gcr_menubar_mainframe() : gcr_menubar_base()
{
	menuFile = new wxMenu;
	menuFileRecentFiles = new wxMenu;
	menuFileRecentDirectories = new wxMenu;

	menuView = new wxMenu;

	menuFilters = new wxMenu;
	menuFiltersBlur = new wxMenu;
	menuFiltersEnhance = new wxMenu;
	menuFiltersColorAdjustment = new wxMenu;
	menuFiltersNoise = new wxMenu;
	menuFiltersAi = new wxMenu;
	menuFiltersSpecial = new wxMenu;

	menuRender = new wxMenu;

	menuSettings = new wxMenu;

	menuHelp = new wxMenu;

	menuFile->Append(gcr_OPEN_FILES, "&Open file(s)\tCtrl+O");
	menuFile->Append(gcr_OPEN_DIRECTORY, "&Open directory\tCtrl+L");
	menuFile->AppendSubMenu(menuFileRecentFiles, "&Recent file(s)");
	menuFile->AppendSubMenu(menuFileRecentDirectories, "&Recent directories");
	menuFile->Append(gcr_EXIT, "&Exit\tCtrl-X");

	menuView->Append(wxID_ANY, "&Minimal interface\tTAB");
	menuView->Append(wxID_ANY, "&Borderless window\tCtrl+TAB");
	menuView->Append(wxID_ANY, "&Fullscreen\tShift+TAB");

	menuFilters->AppendSubMenu(menuFiltersBlur, "&Blur");
	menuFiltersBlur->Append(gcr_FILTER_BLUR, "&Blur");

	menuFilters->AppendSubMenu(menuFiltersEnhance, "&Enhance");
	menuFiltersEnhance->Append(wxID_ANY, "&Sharpen");

	menuFilters->AppendSubMenu(menuFiltersColorAdjustment, "&Color adjustment");
	menuFiltersColorAdjustment->Append(gcr_FILTER_LEVELS, "&Levels");
	menuFiltersColorAdjustment->Append(wxID_ANY, "&HSV");

	menuFilters->AppendSubMenu(menuFiltersNoise, "&Noise");
	menuFiltersNoise->Append(wxID_ANY, "&Noise");

	menuFilters->AppendSubMenu(menuFiltersAi, "&AI");
	menuFiltersAi->Append(gcr_FILTER_FACESWAP, "&Faceswap");
	menuFiltersAi->Append(wxID_ANY, "&Stylizer");

	menuFilters->AppendSubMenu(menuFiltersSpecial, "&Special");
	menuFiltersSpecial->Append(wxID_ANY, "&Interpolation");

	menuRender->Append(wxID_ANY, "&Render image\tF12");
	menuRender->Append(wxID_ANY, "&Render video\tF11");
	menuRender->Append(wxID_ANY, "&Render range\tF10");

	menuSettings->Append(gcr_PREFERENCES, "&Preferences\tCtrl+P");

	menuHelp->Append(gcr_ABOUT, "&About");

	this->Append(menuFile, "&File");
	this->Append(menuView, "&View");
	this->Append(menuFilters, "&Filters");
	this->Append(menuRender, "&Render");
	this->Append(menuSettings, "&Settings");
	this->Append(menuHelp, "&Help");
}

gcr_menubar_mainframe::~gcr_menubar_mainframe()
{
}