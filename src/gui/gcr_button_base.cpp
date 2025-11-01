#include "gcr_button_base.h"

#include <wx/dcbuffer.h>

gcr_button_base::gcr_button_base(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size) :
	wxButton(parent, id, label, pos, size)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
	apply_theme(gcr_buttonstyle_classic_3d);

	Bind(wxEVT_PAINT, &gcr_button_base::on_paint, this);

    Bind(wxEVT_SIZE, [=](wxSizeEvent& event) {
        Refresh();
        event.Skip();
        });

    Bind(wxEVT_ENTER_WINDOW, &gcr_button_base::on_mouse_enter, this);
    Bind(wxEVT_LEAVE_WINDOW, &gcr_button_base::on_mouse_leave, this);

    Bind(wxEVT_LEFT_DOWN, &gcr_button_base::on_left_down, this);
    Bind(wxEVT_LEFT_UP, &gcr_button_base::on_left_up, this);
}

gcr_button_base::~gcr_button_base()
{
}

void gcr_button_base::apply_theme(int selection)
{
	m_buttonstyle = selection;
	this->Refresh();
}

void gcr_button_base::on_paint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);
    wxSize client_size = GetClientSize();
    dc.Clear();

    switch (m_buttonstyle) {
    case gcr_buttonstyle_modern_flat: {
		dc.FloodFill(client_size.x, client_size.y, *wxBLACK);
        break;
    }
    case gcr_buttonstyle_classic_3d: {
        if (m_hovered) {
			wxColour click_color = m_pressed ? wxColour(128, 128, 224) : wxColour(32, 32, 255);
            dc.GradientFillLinear(wxRect(0, 0, client_size.x, client_size.y), click_color, *wxWHITE, wxSOUTH);

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

            dc.SetTextForeground(*wxWHITE);
            dc.DrawText(GetLabel(), (client_size.x - dc.GetTextExtent(GetLabel()).x) / 2,
                (client_size.y - dc.GetTextExtent(GetLabel()).y) / 2);
		}
        else {
            dc.GradientFillLinear(wxRect(0, 0, client_size.x, client_size.y), *wxBLUE, wxColour(128,128,255), wxSOUTH);

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

            dc.SetTextForeground(*wxWHITE);
            dc.DrawText(GetLabel(), (client_size.x - dc.GetTextExtent(GetLabel()).x) / 2,
                (client_size.y - dc.GetTextExtent(GetLabel()).y) / 2);
        }

        if (m_toggle_state > 0) {
            // Draw toggled state indicator
            dc.SetBrush(wxBrush(wxColour(0, 255, 0, 128)));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawCircle(client_size.x - 10, 9, 3);
		}
        break;
    }
    case gcr_buttonstyle_tape: {
        break;
    }
    case gcr_buttonstyle_neon_glow: {
        break;
    }
    case gcr_buttonstyle_pixel_art: {
        break;
    }
    case gcr_buttonstyle_embossed: {
        break;
    }
    }
}

void gcr_button_base::on_mouse_enter(wxMouseEvent& event)
{
    if (!m_hovered) {
        m_hovered = true;
        Refresh();
    }
    event.Skip();
}

void gcr_button_base::on_mouse_leave(wxMouseEvent& event)
{
    if (m_hovered) {
        m_hovered = false;
        Refresh();
    }
    event.Skip();
}

void gcr_button_base::on_left_down(wxMouseEvent& event)
{
    if (!m_pressed) {
        m_pressed = true;
        // Capture to continue receiving mouse up even when cursor leaves the control
        if (!HasCapture()) CaptureMouse();
        Refresh();

        if (on_left_down_cb) on_left_down_cb(event);

        // Synthesize a proper wxEVT_BUTTON and post it so parent handlers run.
        wxCommandEvent btnEvt(wxEVT_BUTTON, GetId());
        btnEvt.SetEventObject(this);

        // Post to parent so normal command routing applies (panel/frames will receive it)
        if (wxWindow* parent = GetParent()) {
            wxPostEvent(parent, btnEvt);
        }
        else {
            // fallback: process on this control
            ProcessWindowEvent(btnEvt);
        }
    }
    // Let wxWidgets handle focus / keyboard etc.
    event.Skip();
}

void gcr_button_base::on_left_up(wxMouseEvent& event)
{
    if (m_pressed) {
        m_pressed = false;
        if (HasCapture()) ReleaseMouse();
        Refresh();

        if (on_left_up_cb) on_left_up_cb(event); // Call user-defined click handler if any
    }
    // Allow default button behaviour (generate wxEVT_BUTTON)
    event.Skip();
}