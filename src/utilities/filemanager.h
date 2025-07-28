#pragma once
#include <filesystem>
#include <vector>

class FileManager
{
public:
	FileManager();
	~FileManager();

	std::filesystem::path directory;
	std::vector<std::filesystem::path> filePaths;

	int recursiveMode;
	int sortMode;

	enum //SORT
	{
		SORT_NAME,
		SORT_DATE,
		SORT_SIZE,
		SORT_TYPE,
	};

	// Add all the files from the directory to the filePaths vector.
	// Returns 0 on success.
	int OpenDirectory();

	// Sorts the filePaths vector according to the sortMode integer.
	// Returns 0 on success.
	int Sort();

	bool IsSupportedFile(std::filesystem::path path);
};

#include <wx/wxprec.h>
#include <wx/collpane.h>
#include <wx/dataview.h>
#include <wx/filectrl.h>

#include "ids.h"
#include "../gui/panel_files.h"
#include "../gui/panel_files_settings.h"
#include "../gui/panel_playlist.h"
#include "../gui/panel_mediactrls.h"
#include "../utilities/img_utils.h"

class FileManagerExtended
{
public:
	FileManagerExtended(PanelFiles& panelFiles,
						PanelPlaylist& panelPlaylist,
						PanelFilesSettings& panelFilesSettings,
						PanelMediaCtrls& panelMediaCtrls);
	~FileManagerExtended();

	std::vector<std::filesystem::path> filePaths;
	std::filesystem::path directory;

	PanelFiles& panelFiles;
	PanelPlaylist& panelPlaylist;
	PanelFilesSettings& panelFilesSettings;
	PanelMediaCtrls& panelMediaCtrls;

	std::vector<wxButton*> arrBitmapButtons;
	std::vector<wxButton*> arrContentButtons;

	int sortMode;
	int viewMode;
	int groupMode;

	enum //SORTMODE
	{
		NAME,
		DATE,
		SIZE,
		TYPE
	};

	enum //VIEW
	{
		VIEW_GRID,
		VIEW_CONTENT,
		VIEW_DETAIL,
		VIEW_TREE
	};

	enum //GROUP
	{
		GROUP_NONE,
		GROUP_TYPE,
		GROUP_DATE,
		GROUP_SIZE
	};

	enum //MEDIATYPE
	{
		IMAGE,
		VIDEO,
		WEBP
	};

	void RefreshAll();

	void SortBitmapButtons();
	void SortContentButtons();
	void SortFilePaths();

	void ViewModeRefresh();

	void CreateBitmapButtons();

	void CreateContentButtons();
};

class DataViewModelDetailsNode
{
public:
	DataViewModelDetailsNode(const wxString& fileName, wxString size, DataViewModelDetailsNode* parent = nullptr) :
		fileName(fileName), size(size), parent(parent) { }
	~DataViewModelDetailsNode() = default;

	wxString fileName;
	wxString size;
	DataViewModelDetailsNode* parent = nullptr;
	std::vector<std::unique_ptr<DataViewModelDetailsNode>> children;

	bool IsContainer() const { return !children.empty(); }
};

class DataViewModelDetails : public wxDataViewModel
{
public:
	DataViewModelDetails();
	~DataViewModelDetails();

	void AddItem(wxString fileName, wxString size);
	void AddFolder(std::unique_ptr<DataViewModelDetailsNode> folder);
	void ExpandAll(wxDataViewCtrl* dataCtrl, const wxDataViewItem& parent = wxDataViewItem(nullptr));

	virtual void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const wxOVERRIDE;
	virtual bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col) wxOVERRIDE;

	virtual bool IsEnabled(const wxDataViewItem& item, unsigned int col) const wxOVERRIDE;

	virtual wxDataViewItem GetParent(const wxDataViewItem& item) const wxOVERRIDE;
	virtual bool IsContainer(const wxDataViewItem& item) const wxOVERRIDE;
	virtual unsigned int GetChildren(const wxDataViewItem& parent, wxDataViewItemArray& array) const wxOVERRIDE;

	DataViewModelDetailsNode* GetRoot() const { return m_root.get(); }

private:
	std::unique_ptr<DataViewModelDetailsNode> m_root;
};