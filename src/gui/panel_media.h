#pragma once
#include <wx/wxprec.h>

class PanelMedia : public wxPanel
{
public:
	PanelMedia(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelMedia();

	wxBoxSizer* boxSizerMaster;
};