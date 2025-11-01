#pragma once
#include <wx/wxprec.h>
#include "../gcr_panel_base.h"

class gcr_panel_screen : public gcr_panel_base
{
public:
	gcr_panel_screen(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~gcr_panel_screen();

	wxBoxSizer* m_main_sizer;
};