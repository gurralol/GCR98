#include "gcr_choice_base.h"

#include <wx/dcbuffer.h>

gcr_choice_base::gcr_choice_base(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxChoice(parent, id, pos, size)
{
	m_choice_style = gcr_choicestyle_modern_flat;
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	Bind(wxEVT_PAINT, &gcr_choice_base::on_paint, this);
	Bind(wxEVT_ENTER_WINDOW, &gcr_choice_base::on_mouse_enter, this);
	Bind(wxEVT_LEAVE_WINDOW, &gcr_choice_base::on_mouse_leave, this);
	Bind(wxEVT_LEFT_DOWN, &gcr_choice_base::on_left_down, this);
	Bind(wxEVT_LEFT_UP, &gcr_choice_base::on_left_up, this);
}

gcr_choice_base::~gcr_choice_base()
{
}

void gcr_choice_base::apply_theme(int selection)
{
	m_choice_style = selection;
	Refresh();
}

void gcr_choice_base::on_paint(wxPaintEvent&)
{
	wxAutoBufferedPaintDC dc(this);
	wxSize size = GetClientSize();
	if (m_hovered) {
		wxColour click_color = m_pressed ? wxColour(128, 128, 224) : wxColour(32, 32, 255);
		dc.GradientFillLinear(wxRect(0, 0, size.x, size.y), click_color, *wxWHITE, wxSOUTH);

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
		dc.DrawText(GetLabel(), (size.x - dc.GetTextExtent(GetLabel()).x) / 2,
			(size.y - dc.GetTextExtent(GetLabel()).y) / 2);
	}
	else {
		dc.GradientFillLinear(wxRect(0, 0, size.x, size.y), *wxBLUE, wxColour(128, 128, 255), wxSOUTH);

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
		dc.DrawText(GetLabel(), (size.x - dc.GetTextExtent(GetLabel()).x) / 2,
			(size.y - dc.GetTextExtent(GetLabel()).y) / 2);
	}
	dc.SetBrush(wxBrush(GetBackgroundColour()));
	dc.SetPen(*wxTRANSPARENT_PEN);
	//dc.DrawRectangle(0, 0, size.x, size.y);
	wxString selected_text = "";
	int selection = GetSelection();
	if (selection != wxNOT_FOUND)
	{
		selected_text = GetString(selection);
	}
	wxFont font = GetFont();
	dc.SetFont(font);
	dc.SetTextForeground(wxColour(255,255,255));
	wxCoord text_width, text_height;
	dc.GetTextExtent(selected_text, &text_width, &text_height);
	int padding = 10;
	int arrow_size = 10;
	int arrow_x = size.x - padding - arrow_size;
	int arrow_y = (size.y - arrow_size) / 2;
	// Draw the selected text
	dc.DrawText(selected_text, padding, (size.y - text_height) / 2);
	// Draw the dropdown arrow
	wxPoint points[3];
	points[0] = wxPoint(arrow_x, arrow_y);
	points[1] = wxPoint(arrow_x + arrow_size, arrow_y);
	points[2] = wxPoint(arrow_x + arrow_size / 2, arrow_y + arrow_size);
	dc.SetBrush(m_hovered ? *wxLIGHT_GREY_BRUSH : *wxBLACK_BRUSH);
	dc.DrawPolygon(3, points);
	// Draw border
	dc.SetPen(wxPen(*wxBLACK, 1));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(0, 0, size.x, size.y);
}

void gcr_choice_base::on_mouse_enter(wxMouseEvent& event)
{
	m_hovered = true;
	Refresh();
	event.Skip();
}

void gcr_choice_base::on_mouse_leave(wxMouseEvent& event)
{
	m_hovered = false;
	Refresh();
	event.Skip();
}

void gcr_choice_base::on_left_down(wxMouseEvent& event)
{
	m_pressed = true;
	Refresh();
	event.Skip();
}

void gcr_choice_base::on_left_up(wxMouseEvent& event)
{
	m_pressed = false;
	Refresh();
	event.Skip();
}