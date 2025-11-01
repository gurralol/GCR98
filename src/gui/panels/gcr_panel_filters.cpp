#include "gcr_panel_filters.h"

gcr_panel_filters::gcr_panel_filters(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) : gcr_panel_base(parent, id, pos, size)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);
	m_buttons_sizer = new wxGridSizer(2, 0, 0);
	m_filters_sizer = new wxBoxSizer(wxVERTICAL);

	wxSize btn_size = wxSize(150, 36);
	btn_disable_all = new gcr_button_base(this, wxID_ANY, "disable all", wxPoint(0, 0), btn_size);
	m_buttons_sizer->Add(btn_disable_all, 1, wxEXPAND, 0);
	btn_enable_all = new gcr_button_base(this, wxID_ANY, "enable all", wxPoint(0, 0), btn_size);
	m_buttons_sizer->Add(btn_enable_all, 1, wxEXPAND, 0);
	btn_reset_all = new gcr_button_base(this, wxID_ANY, "reset all", wxPoint(0, 0), btn_size);
	m_buttons_sizer->Add(btn_reset_all, 1, wxEXPAND, 0);
	btn_remove_all = new gcr_button_base(this, wxID_ANY, "remove all", wxPoint(0, 0), btn_size);
	m_buttons_sizer->Add(btn_remove_all, 1, wxEXPAND, 0);
	btn_save_stack = new gcr_button_base(this, wxID_ANY, "save stack", wxPoint(0, 0), btn_size);
	m_buttons_sizer->Add(btn_save_stack, 1, wxEXPAND, 0);
	btn_load_stack = new gcr_button_base(this, wxID_ANY, "load stack", wxPoint(0, 0), btn_size);
	m_buttons_sizer->Add(btn_load_stack, 1, wxEXPAND, 0);

	m_main_sizer->Add(m_buttons_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 3);

	m_filters_scroll_window = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);
	m_filters_scroll_window->SetScrollRate(5, 5);
	m_filters_scroll_window->SetSizer(m_filters_sizer);

	m_main_sizer->Add(m_filters_scroll_window, 1, wxEXPAND | wxALL, 3);
}

gcr_panel_filters::~gcr_panel_filters()
{
}
