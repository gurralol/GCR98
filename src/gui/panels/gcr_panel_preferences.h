#pragma once
#include <wx/wxprec.h>
#include "../gcr_panel_base.h"

class gcr_panel_preferences : public gcr_panel_base
{
public:
	gcr_panel_preferences(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~gcr_panel_preferences();

	wxBoxSizer* m_main_sizer;
	wxChoice* m_choice_theme;
	wxChoice* m_choice_borderstyle;
	wxChoice* m_dockart_choice;

private:

};