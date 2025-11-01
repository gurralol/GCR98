#pragma once
#include <wx/wxprec.h>
#include "../gcr_panel_base.h"

class gcr_panel_files : public gcr_panel_base
{
public:
	gcr_panel_files(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~gcr_panel_files();

	wxBoxSizer* m_main_sizer;
	wxGridSizer* m_files_sizer;
};