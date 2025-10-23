#pragma once
#include <wx/wxprec.h>

class gcr_button_base : public wxButton
{
public:
	gcr_button_base(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size);
	~gcr_button_base();

	enum gcr_button_style
	{
		gcr_buttonstyle_modern_flat = 0,
		gcr_buttonstyle_classic_3d = 1,
		gcr_buttonstyle_tape = 2,
		gcr_buttonstyle_neon_glow = 3,
		gcr_buttonstyle_pixel_art = 4,
		gcr_buttonstyle_embossed = 5
	};

	void apply_theme(int selection);

private:
	int m_buttonstyle;

	void on_paint(wxPaintEvent&);

	bool m_hovered = false;
	void on_mouse_enter(wxMouseEvent& event);
	void on_mouse_leave(wxMouseEvent& event);

	bool m_pressed = false;
	void on_left_down(wxMouseEvent& event);
	void on_left_up(wxMouseEvent& event);
};