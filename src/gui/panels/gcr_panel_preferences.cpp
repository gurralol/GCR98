#include "gcr_panel_preferences.h"

gcr_panel_preferences::gcr_panel_preferences(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) : gcr_panel_base(parent, id, pos, size)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);

	wxBoxSizer* sizer_theme = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* label_theme = new wxStaticText(this, wxID_ANY, wxT("Select theme:"));
	m_choice_theme = new wxChoice(this, wxID_ANY);
	m_choice_theme->Append(wxT("Midnight Mix"));
	m_choice_theme->Append(wxT("Sunburst"));
	m_choice_theme->Append(wxT("Retro Green"));
	m_choice_theme->Append(wxT("VHS Static"));
	m_choice_theme->Append(wxT("Classic 98"));
	m_choice_theme->Append(wxT("Neon Dreams"));
	m_choice_theme->SetSelection(0);
	sizer_theme->Add(label_theme, 0, wxALIGN_CENTER, 5);
	sizer_theme->AddStretchSpacer(1);
	sizer_theme->Add(m_choice_theme, 0, wxALIGN_CENTER, 5);
	m_main_sizer->Add(sizer_theme, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* sizer_borderstyle = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* label_borderstyle = new wxStaticText(this, wxID_ANY, wxT("Select border style:"));
	m_choice_borderstyle = new wxChoice(this, wxID_ANY);
	m_choice_borderstyle->Append(wxT("Frameless"));
	m_choice_borderstyle->Append(wxT("Plastic Ridge"));
	m_choice_borderstyle->Append(wxT("Chrome Trim"));
	m_choice_borderstyle->Append(wxT("Shadowbox"));
	m_choice_borderstyle->Append(wxT("Pixel Edge"));
	m_choice_borderstyle->Append(wxT("Embossed"));
	m_choice_borderstyle->SetSelection(0);
	sizer_borderstyle->Add(label_borderstyle, 0, wxALIGN_CENTER, 5);
	sizer_borderstyle->AddStretchSpacer(1);
	sizer_borderstyle->Add(m_choice_borderstyle, 0, wxALIGN_CENTER, 5);
	m_main_sizer->Add(sizer_borderstyle, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* sizer_buttonstyle = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* label_buttonstyle = new wxStaticText(this, wxID_ANY, wxT("Select button style:"));
	m_choice_buttonstyle = new wxChoice(this, wxID_ANY);
	m_choice_buttonstyle->Append(wxT("Flat Minimal"));
	m_choice_buttonstyle->Append(wxT("3D Glossy"));
	m_choice_buttonstyle->Append(wxT("Neon Glow"));
	m_choice_buttonstyle->Append(wxT("Retro Pixel"));
	m_choice_buttonstyle->SetSelection(0);
	sizer_buttonstyle->Add(label_buttonstyle, 0, wxALIGN_CENTER, 5);
	sizer_buttonstyle->AddStretchSpacer(1);
	sizer_buttonstyle->Add(m_choice_buttonstyle, 0, wxALIGN_CENTER, 5);
	m_main_sizer->Add(sizer_buttonstyle, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* sizer_font = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* label_font = new wxStaticText(this, wxID_ANY, wxT("Select font:"));
	m_choice_font = new wxChoice(this, wxID_ANY);
	m_choice_font->Append(wxT("Arial"));
	m_choice_font->Append(wxT("Verdana"));
	m_choice_font->Append(wxT("Tahoma"));
	m_choice_font->Append(wxT("Comic Sans MS"));
	m_choice_font->Append(wxT("Courier New"));
	m_choice_font->SetSelection(0);
	sizer_font->Add(label_font, 0, wxALIGN_CENTER, 5);
	sizer_font->AddStretchSpacer(1);
	sizer_font->Add(m_choice_font, 0, wxALIGN_CENTER, 5);
	m_main_sizer->Add(sizer_font, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* sizer_dockart = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* label_dockart = new wxStaticText(this, wxID_ANY, wxT("Select dock art style:"));
	m_choice_dockart = new wxChoice(this, wxID_ANY);
	m_choice_dockart->Append(wxT("Default"));
	m_choice_dockart->Append(wxT("Classic"));
	m_choice_dockart->SetSelection(0);
	sizer_dockart->Add(label_dockart, 0, wxALIGN_CENTER, 5);
	sizer_dockart->AddStretchSpacer(1);
	sizer_dockart->Add(m_choice_dockart, 0, wxALIGN_CENTER, 5);
	m_main_sizer->Add(sizer_dockart, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* sizer_vcr = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* label_vcr = new wxStaticText(this, wxID_ANY, wxT("Select VCR style:"));
	m_choice_vcr = new wxChoice(this, wxID_ANY);
	m_choice_vcr->Append(wxT("Default"));
	m_choice_vcr->Append(wxT("Classic"));
	m_choice_vcr->SetSelection(0);
	sizer_vcr->Add(label_vcr, 0, wxALIGN_CENTER, 5);
	sizer_vcr->AddStretchSpacer(1);
	sizer_vcr->Add(m_choice_vcr, 0, wxALIGN_CENTER, 5);
	m_main_sizer->Add(sizer_vcr, 0, wxEXPAND | wxALL, 5);

	this->Layout();
}

gcr_panel_preferences::~gcr_panel_preferences()
{
}
