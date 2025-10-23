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

	this->Layout();
}

gcr_panel_preferences::~gcr_panel_preferences()
{
}
