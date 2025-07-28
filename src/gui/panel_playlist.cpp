#include "panel_playlist.h"

PanelPlaylist::PanelPlaylist(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size, wxBORDER_RAISED)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	boxSizerTop = new wxBoxSizer(wxHORIZONTAL);
	this->SetSizer(boxSizerMaster);

	textPlaylist = new wxStaticText(this, wxID_ANY, "Playlist", wxDefaultPosition, wxSize(50, 16));
	staticLinePlaylist = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(32, 1));

	btnRemove = new wxButton(this, wxID_ANY, "Remove", wxDefaultPosition, wxSize(56, 22));
	btnClear = new wxButton(this, wxID_ANY, "Clear", wxDefaultPosition, wxSize(42, 22));

	listCtrlPlaylist = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_HEADER);
	listCtrlPlaylist->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 136);
	listCtrlPlaylist->InsertColumn(1, "Duration", wxLIST_FORMAT_LEFT, 58);
	/*
	wxListItem listItemDummy1;
	listItemDummy1.SetId(0);
	listItemDummy1.SetText("Filename123.mp4");

	wxListItem listItemDummy2;
	listItemDummy2.SetId(1);
	listItemDummy2.SetText("FileName321.png");


	listCtrlPlaylist->InsertItem(listItemDummy1);
	listCtrlPlaylist->InsertItem(listItemDummy2);

	listCtrlPlaylist->SetItem(0, 1, "02:00:00");
	listCtrlPlaylist->SetItem(1, 1, "04:00");
	*/
	boxSizerMaster->Add(boxSizerTop, 0, wxEXPAND | wxBOTTOM, 2);
	boxSizerTop->Add(textPlaylist, 0, wxALIGN_CENTER);
	boxSizerTop->AddStretchSpacer();
	boxSizerTop->Add(btnRemove, 0, wxALIGN_CENTER);
	boxSizerTop->Add(btnClear, 0, wxALIGN_CENTER);
	boxSizerMaster->Add(staticLinePlaylist, 0, wxEXPAND | wxBOTTOM, 4);
	boxSizerMaster->Add(listCtrlPlaylist, 1, wxEXPAND);
}

PanelPlaylist::~PanelPlaylist()
{

}
