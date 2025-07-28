#pragma once
#include <wx/wxprec.h>
#include <wx/tglbtn.h>

class PanelFiltersSettings : public wxPanel
{
public:
	PanelFiltersSettings(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelFiltersSettings();

	wxBoxSizer* boxSizerMaster;
	wxBoxSizer* boxSizerCtrls1;
	wxBoxSizer* boxSizerCtrls2;
	wxBoxSizer* boxSizerCtrls3;
	wxBoxSizer* boxSizerCtrls4;

	wxStaticBoxSizer* staticBoxSizer0;
	
	wxChoice* choiceBypassByType;

	wxButton* btnBypassAll;
	wxButton* btnAllowAll;
	wxButton* btnRemoveAll;

	wxButton* btnLoadStack;
	wxButton* btnSaveStack;

	wxButton* btnCollapseAll;
	wxButton* btnExpandAll;
};