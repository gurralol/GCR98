#include "gcr_panel_base.h"

gcr_panel_base::gcr_panel_base(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size)
{
	SetBackgroundStyle(wxBG_STYLE_PAINT);

	apply_theme(gcr_theme_sunburst);
	apply_borderstyle(gcr_borderstyle_plastic_ridge);

	Bind(wxEVT_PAINT, &gcr_panel_base::on_paint, this);

	Bind(wxEVT_SIZE, [=](wxSizeEvent& event) {
			Refresh();
			event.Skip();
		});
}

gcr_panel_base::~gcr_panel_base()
{
}

void gcr_panel_base::apply_theme(int selection)
{
    m_theme = selection;
	this->Refresh();
}

void gcr_panel_base::apply_borderstyle(int selection)
{
	m_borderstyle = selection;
	this->Refresh();
}

void gcr_panel_base::on_paint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    wxSize client_size = GetClientSize();

    switch (m_theme) {
        case gcr_theme_midnight_mix: {
            break;
        }
        case gcr_theme_sunburst: {
            dc.GradientFillLinear(wxRect(0, 0, client_size.x, client_size.y), *wxBLUE, *wxWHITE, wxSOUTH);
            break;
        }
        case gcr_retro_green: {
            break;
        }
        case gcr_vhs_static: {
            break;
        }
        case gcr_classic_98: {
            break;
        }
        case gcr_theme_neon_dreams: {
            break;
        }
    }

    switch (m_borderstyle) {
        case gcr_borderstyle_frameless: {
            break;
        }
        case gcr_borderstyle_plastic_ridge: {
            wxRect r = GetClientRect();
            dc.SetBrush(wxBrush(GetBackgroundColour()));
            dc.SetPen(*wxTRANSPARENT_PEN);

            // Draw "raised" effect
            dc.SetPen(wxPen(wxColour(64,64,64), 6));
            dc.DrawLine(r.GetLeft(), r.GetBottom(), r.GetLeft(), r.GetTop());
            dc.DrawLine(r.GetLeft(), r.GetTop(), r.GetRight(), r.GetTop());

            dc.SetPen(wxPen(wxColour(16, 16, 16), 6));
            dc.DrawLine(r.GetRight(), r.GetTop(), r.GetRight(), r.GetBottom());
            dc.DrawLine(r.GetLeft(), r.GetBottom(), r.GetRight(), r.GetBottom());
            break;
        }
        case gcr_borderstyle_chrome_trim: {
            break;
        }
        case gcr_borderstyle_shadowbox: {
            break;
        }
        case gcr_borderstyle_pixel_edge: {
            break;
        }
        case gcr_borderstyle_embossed: {
            break;
        }
    }
}