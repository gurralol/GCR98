#include "gcr_panel_files.h"

gcr_panel_files::gcr_panel_files(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) 
	: gcr_panel_base(parent, id, pos, size)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);

	m_files_sizer = new wxGridSizer(0, 2, 0, 0);
	m_main_sizer->Add(m_files_sizer, 1, wxEXPAND | wxALL, 5);
}

gcr_panel_files::~gcr_panel_files()
{
}
