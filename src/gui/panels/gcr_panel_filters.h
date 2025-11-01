#pragma once
#include <wx/wxprec.h>
#include "../gcr_panel_base.h"

class gcr_panel_filters : public gcr_panel_base
{
public:
	gcr_panel_filters(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~gcr_panel_filters();

	wxBoxSizer* m_main_sizer;
};