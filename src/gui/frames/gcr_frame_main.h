#pragma once
// wxWidgets
#include <wx/wxprec.h>
//#include <wx/aui/aui.h>

// GUI
#include "../menubars/gcr_menubar_mainframe.h"
#include "../frames/gcr_frame_preferences.h"
#include "../../utilities/gcr_aui_manager.h"

#include "../panels/gcr_panel_mediactrls.h"

class gcr_frame_main : public gcr_frame_base
{
public:
	gcr_frame_main(const wxString& title);
	~gcr_frame_main();

	gcr_menubar_mainframe* m_menubar;

	wxBoxSizer* m_main_sizer;
	gcr_panel_mediactrls* m_panel_mediactrls;

	gcr_aui_manager* m_aui_manager;

	gcr_frame_preferences* m_frame_preferences;

	void bind_events();
    void set_window_settings_recursively(wxWindow* parent);

	// Menu Bar Events
	void menubar_exit_onclick(wxCommandEvent& event);
	void menubar_settings_onclick(wxCommandEvent& event);
	void menubar_about_onclick(wxCommandEvent& event);
    
    wxDECLARE_EVENT_TABLE();
};

