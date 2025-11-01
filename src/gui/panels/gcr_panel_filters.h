#pragma once
#include <wx/wxprec.h>
#include <wx/scrolwin.h>
#include "../gcr_panel_base.h"
#include "../gcr_button_base.h"

class gcr_panel_filters : public gcr_panel_base
{
public:
	gcr_panel_filters(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~gcr_panel_filters();

	wxBoxSizer* m_main_sizer;
	wxGridSizer* m_buttons_sizer;
	wxScrolledWindow* m_filters_scroll_window;
	wxBoxSizer* m_filters_sizer;

	gcr_button_base* btn_disable_all;
	gcr_button_base* btn_enable_all;
	gcr_button_base* btn_reset_all;
	gcr_button_base* btn_remove_all;
	gcr_button_base* btn_save_stack;
	gcr_button_base* btn_load_stack;
};