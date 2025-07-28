#pragma once
#include <wx/wxprec.h>

class PanelFilesSettings : public wxPanel
{
public:
	PanelFilesSettings(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelFilesSettings();

	wxBoxSizer* boxSizerMaster;
	wxBoxSizer* boxSizerCtrls1;
	wxBoxSizer* boxSizerCtrls2;

	wxStaticBoxSizer* staticBoxSizerRecursive;
	wxStaticBoxSizer* staticBoxSizerView;
	wxStaticBoxSizer* staticBoxSizerSort;
	wxStaticBoxSizer* staticBoxSizerGroup;

	wxChoice* choiceRecursive;
	wxChoice* choiceView;
	wxChoice* choiceSort;
	wxChoice* choiceGroup;
};