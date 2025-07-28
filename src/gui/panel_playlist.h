#pragma once
#include <wx/wxprec.h>
#include <wx/listctrl.h>
#include <wx/statline.h>

class PanelPlaylist : public wxPanel
{
public:
	PanelPlaylist(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelPlaylist();

	wxBoxSizer* boxSizerMaster;
	wxBoxSizer* boxSizerTop;

	wxButton* btnRemove;
	wxButton* btnClear;

	wxStaticText* textPlaylist;
	wxStaticLine* staticLinePlaylist;

	wxListCtrl* listCtrlPlaylist;
};