#pragma once
// wxWidgets
#include <wx/wxprec.h>
#include <wx/dir.h>

// Utilities
#include "../../utilities/img_utils.h"

// GUI
#include "../menubars/gcr_menubar_mainframe.h"

#include "../frames/gcr_frame_preferences.h"

#include "../../utilities/gcr_aui_manager.h"

#include "../../utilities/file_manager.h"

#include "../panels/gcr_panel_files.h"
#include "../panels/gcr_panel_screen.h"
#include "../panels/gcr_panel_mediactrls.h"
#include "../panels/gcr_panel_filters.h"

class gcr_frame_main : public gcr_frame_base
{
public:
	gcr_frame_main(const wxString& title);
	~gcr_frame_main();

	// Menu Bar
	gcr_menubar_mainframe* m_menubar;

	// AUI Manager
	gcr_aui_manager* m_aui_manager;

	// Preferences Frame
	gcr_frame_preferences* m_frame_preferences;

	// File Manager
	file_manager* m_file_manager;

	// Main Sizer for the Frame
	wxBoxSizer* m_main_sizer;

	// Panels
	gcr_panel_files* m_panel_files;
	gcr_panel_screen* m_panel_screen;
	gcr_panel_mediactrls* m_panel_mediactrls;
	gcr_panel_filters* m_panel_filters;

	// Menu Bar Events
	void menubar_open_directory_onclick(wxCommandEvent& event);
	void menubar_exit_onclick(wxCommandEvent& event);
	void menubar_settings_onclick(wxCommandEvent& event);
	void menubar_about_onclick(wxCommandEvent& event);
    
	// Utilities
    void set_window_settings_recursively(wxWindow* parent);
	void add_file_buttons_to_files_panel();
    
	wxDECLARE_EVENT_TABLE();
};

