#pragma once
#include <wx/wxprec.h>
#include <wx/scrolwin.h>
#include "../gcr_panel_base.h"
#include "../gcr_choice_base.h"

class gcr_panel_files : public gcr_panel_base
{
public:
	gcr_panel_files(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~gcr_panel_files();

	wxBoxSizer* m_main_sizer;
	wxGridSizer* m_toolbar_sizer;
	wxScrolledWindow* m_scrolled_window;

	gcr_choice_base* m_sorting_options;
	gcr_choice_base* m_filter_file_types;
	gcr_choice_base* m_view_modes;
	gcr_choice_base* m_grouping_options;
};