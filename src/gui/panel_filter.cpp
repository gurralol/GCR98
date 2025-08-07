#include "panel_filter.h"

PanelFilter::PanelFilter(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size, wxBORDER_DOUBLE)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);

	boxSizerTopbar = new wxBoxSizer(wxHORIZONTAL);
	boxSizerPresets = new wxBoxSizer(wxHORIZONTAL);
	boxSizerContent = new wxBoxSizer(wxVERTICAL);
	//boxSizerAuthor = new wxBoxSizer(wxHORIZONTAL);

	panelTopbar = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(32, 26), wxBORDER_DOUBLE);
	panelPresets = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(32, 23), wxBORDER_NONE);
	panelContent = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(32, 200));
	panelContent->SetBackgroundColour(wxColor(255, 255, 255));

	staticLine1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(32, 1));

	panelTopbar->SetSizer(boxSizerTopbar);
	panelPresets->SetSizer(boxSizerPresets);
	panelContent->SetSizer(boxSizerContent);

	boxSizerMaster->Add(panelTopbar, 0, wxEXPAND);
	boxSizerMaster->Add(panelPresets, 0, wxEXPAND | wxTOP, 2);
	boxSizerMaster->Add(staticLine1, 0, wxEXPAND | wxTOP | wxBOTTOM, 2);
	boxSizerMaster->Add(panelContent, 1, wxEXPAND);
	//boxSizerMaster->Add(boxSizerAuthor, 0, wxALIGN_RIGHT);

	textTitle = new wxStaticText(panelTopbar, wxID_ANY, "Title", wxDefaultPosition, wxSize(72, 15), wxST_ELLIPSIZE_END);
	wxFont fontTitle;
	fontTitle = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false);
	textTitle->SetFont(fontTitle);

	spinBtnMove = new wxSpinButton(panelTopbar, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	btnMinimize = new wxButton(panelTopbar, wxID_ANY, "-", wxDefaultPosition, wxSize(26, 26));
	btnBypass = new wxToggleButton(panelTopbar, wxID_ANY, "bypass", wxDefaultPosition, wxSize(48, 26));
	btnDestroy = new wxButton(panelTopbar, wxID_ANY, "x", wxDefaultPosition, wxSize(26, 26));

	boxSizerTopbar->Add(textTitle, 0, wxALIGN_CENTER | wxLEFT, 1);
	boxSizerTopbar->AddStretchSpacer();
	boxSizerTopbar->Add(spinBtnMove, 0, wxEXPAND);
	boxSizerTopbar->Add(btnBypass, 0, wxEXPAND);
	boxSizerTopbar->Add(btnMinimize, 0, wxEXPAND);
	boxSizerTopbar->Add(btnDestroy, 0, wxEXPAND);

	textPresets = new wxStaticText(panelPresets, wxID_ANY, "Presets", wxDefaultPosition, wxSize(42, 15), wxST_ELLIPSIZE_END);
	btnPresetSave = new wxButton(panelPresets, wxID_ANY, "Save", wxDefaultPosition, wxSize(36, 12));
	btnPresetDelete = new wxButton(panelPresets, wxID_ANY, "Delete", wxDefaultPosition, wxSize(44, 12));
	wxFont fontPresets;
	fontPresets = wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
	textPresets->SetFont(fontPresets);

	presetArr.push_back("Default");
	presetArr.push_back("Strong");
	presetArr.push_back("Light");
	choicePreset = new wxChoice(panelPresets, wxID_ANY, wxDefaultPosition, wxSize(62, 20), presetArr);
	choicePreset->SetSelection(0);

	boxSizerPresets->Add(textPresets, 0, wxALIGN_CENTER | wxLEFT, 2);
	boxSizerPresets->AddStretchSpacer();
	boxSizerPresets->Add(btnPresetSave, 0, wxEXPAND);
	boxSizerPresets->Add(btnPresetDelete, 0, wxEXPAND);
	boxSizerPresets->Add(choicePreset, 0, wxEXPAND);
	/*
	textName = "Garfield";
	textAuthor = new wxStaticText(this, wxID_ANY, "Created by: " + textName, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
	wxFont fontAuthor;
	fontAuthor = wxFont(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
	textAuthor->SetFont(fontAuthor);

	boxSizerAuthor->Add(textAuthor, 0, wxALIGN_CENTER | wxTOP, 2);*/
}

PanelFilter::~PanelFilter()
{

}

wxImage PanelFilter::ApplyFilter(wxImage img)
{
	return wxImage();
}
