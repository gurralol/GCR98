#pragma once
#include <wx/wxprec.h>
#include "../gcr_frame_base.h"
#include "../panels/gcr_panel_preferences.h"
#include "../../utilities/gcr_aui_manager.h"
#include "../../utilities/theme_manager.h"

class gcr_frame_preferences : public gcr_frame_base
{
public:
	gcr_frame_preferences(const wxString& title);
	~gcr_frame_preferences();

private:
	wxBoxSizer* m_main_sizer;
	gcr_panel_preferences* m_panel_preferences;
	gcr_aui_manager* m_aui_manager;
};