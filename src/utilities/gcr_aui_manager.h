#pragma once
#include <wx/aui/aui.h>

class gcr_aui_manager : public wxAuiManager
{
public:
	gcr_aui_manager(wxWindow* managed_wnd, unsigned int flags = wxAUI_MGR_DEFAULT);
	~gcr_aui_manager();

	enum gcr_dockart_theme
	{
		gcr_theme_midnight_mix = 0,
		gcr_theme_sunburst = 1,
		gcr_retro_green = 2,
		gcr_vhs_static = 3,
		gcr_classic_98 = 4,
		gcr_theme_neon_dreams = 5,
	};

	void apply_theme(int selection);
};