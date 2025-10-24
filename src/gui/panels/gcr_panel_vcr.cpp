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

    wxRect r = GetClientRect();
    dc.SetBrush(wxBrush(GetBackgroundColour()));
    dc.SetPen(*wxTRANSPARENT_PEN);

    // Draw "raised" effect
    dc.SetPen(wxPen(wxColour(64, 64, 64), 6));
    dc.DrawLine(r.GetLeft(), r.GetBottom(), r.GetLeft(), r.GetTop());
    dc.DrawLine(r.GetLeft(), r.GetTop(), r.GetRight(), r.GetTop());

    dc.SetPen(wxPen(wxColour(16, 16, 16), 6));
    dc.DrawLine(r.GetRight(), r.GetTop(), r.GetRight(), r.GetBottom());
    dc.DrawLine(r.GetLeft(), r.GetBottom(), r.GetRight(), r.GetBottom());
}
