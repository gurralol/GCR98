#pragma once
#include <wx/wxprec.h>

class gcr_choice_base : public wxChoice
{
	public:
	gcr_choice_base(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~gcr_choice_base();

	enum gcr_choice_style
	{
		gcr_choicestyle_modern_flat = 0,
		gcr_choicestyle_classic_3d = 1,
		gcr_choicestyle_tape = 2,
		gcr_choicestyle_neon_glow = 3,
		gcr_choicestyle_pixel_art = 4,
		gcr_choicestyle_embossed = 5
	};

	void apply_theme(int selection);

	int m_choice_style;

	virtual void on_paint(wxPaintEvent&);

	bool m_hovered = false;
	void on_mouse_enter(wxMouseEvent& event);
	void on_mouse_leave(wxMouseEvent& event);

	bool m_pressed = false;
	void on_left_down(wxMouseEvent& event);
	void on_left_up(wxMouseEvent& event);
};

