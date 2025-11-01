#include "gcr_button_thumbnail.h"

gcr_button_thumbnail::gcr_button_thumbnail(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size) :
    gcr_button_base(parent, id, label, pos, size)
{
    
}

gcr_button_thumbnail::~gcr_button_thumbnail()
{
}

void gcr_button_thumbnail::on_paint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    wxSize btn_size = GetClientSize();
    if (m_thumbnail_image.IsOk())
    {
        wxImage scaled_image = m_thumbnail_image.Scale(btn_size.GetWidth(), btn_size.GetHeight(), wxIMAGE_QUALITY_HIGH);
        if (m_hovered)
        {
			scaled_image.ChangeBrightness(0.2);
		}
        if (m_pressed)
        {
			scaled_image.ChangeBrightness(-0.2);
        }
        wxBitmap bitmap(scaled_image);
        dc.DrawBitmap(bitmap, 0, 0, false);
    }
    else
    {
        dc.SetBrush(*wxLIGHT_GREY_BRUSH);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(0, 0, btn_size.GetWidth(), btn_size.GetHeight());
	}
}
