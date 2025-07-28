#pragma once
#include <wx/wxprec.h>
#include <wx/grid.h>

class PanelFiles : public wxPanel
{
public:
	PanelFiles(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelFiles();

	wxBoxSizer* boxSizerMaster;

	wxScrolledWindow* scrolledWindowFiles;
};