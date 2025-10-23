#include "gcr_panel_vcr.h"

gcr_panel_vcr::gcr_panel_vcr(wxWindow* parent, wxWindowID id, wxPoint pos, const wxSize size) : gcr_panel_base(parent, id, pos, size)
{
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_mainSizer);
}

gcr_panel_vcr::~gcr_panel_vcr()
{
}

void gcr_panel_vcr::on_paint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc(this);
	wxSize client_size = GetClientSize();

	dc.GradientFillLinear(wxRect(0, 0, client_size.x, client_size.y), *wxBLUE, *wxBLACK, wxNORTH);
}
