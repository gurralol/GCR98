#pragma once
#include <wx/wxprec.h>
#include <wx/dcbuffer.h>

class gcr_panel_base : public wxPanel
{
public:
    gcr_panel_base(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~gcr_panel_base();

    enum gcr_theme
    {
        gcr_theme_midnight_mix = 0,
        gcr_theme_sunburst = 1,
		gcr_retro_green = 2,
		gcr_vhs_static = 3,
		gcr_classic_98 = 4,
		gcr_theme_neon_dreams = 5,
    };

    enum gcr_borderstyle
    {
        gcr_borderstyle_frameless = 0,
		gcr_borderstyle_plastic_ridge = 1,
		gcr_borderstyle_chrome_trim = 2,
		gcr_borderstyle_shadowbox = 3,
		gcr_borderstyle_pixel_edge = 4,
        gcr_borderstyle_embossed = 5
	};

    void apply_theme(int selection);
	void apply_borderstyle(int selection);

private:
    int m_theme = gcr_theme_midnight_mix;
    int m_borderstyle = gcr_borderstyle_frameless;

    virtual void on_paint(wxPaintEvent&);
};
