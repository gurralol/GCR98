#pragma once
#include <wx/wxprec.h>

class PanelFilters : public wxPanel
{
public:
	PanelFilters(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelFilters();

	wxBoxSizer* boxSizerMaster;
};