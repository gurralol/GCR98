#include "gcr_panel_playlist.h"

gcr_panel_playlist::gcr_panel_playlist(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size)
	: gcr_panel_base(parent, id, pos, size)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);
}

gcr_panel_playlist::~gcr_panel_playlist()
{
}
