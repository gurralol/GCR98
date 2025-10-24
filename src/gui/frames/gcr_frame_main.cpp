#pragma once
#include "gcr_frame_main.h"

#include "../../utilities/theme_manager.h"

wxBEGIN_EVENT_TABLE(gcr_frame_main, wxFrame)
	EVT_MENU(gcr_EXIT, menubar_exit_onclick)
	EVT_MENU(gcr_PREFERENCES, menubar_settings_onclick)
	EVT_MENU(gcr_ABOUT, menubar_about_onclick)
wxEND_EVENT_TABLE()

gcr_frame_main::gcr_frame_main(const wxString& title) : gcr_frame_base(title)
{
    wxInitAllImageHandlers();
	this->SetBackgroundColour(wxColour(0, 0, 0));
	this->SetForegroundColour(wxColour(0, 0, 0));

	set_window_settings_recursively(this);

	m_menubar = new gcr_menubar_mainframe();
	SetMenuBar(m_menubar);

	// Main Sizer
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);

	// AUI Manager
	m_aui_manager = new gcr_aui_manager(this, wxAUI_MGR_DEFAULT | wxAUI_MGR_LIVE_RESIZE);

	gcr_panel_base* panel1 = new gcr_panel_base(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gcr_panel_base* panel2 = new gcr_panel_base(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	gcr_panel_base* panel3 = new gcr_panel_base(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_panel_mediactrls = new gcr_panel_mediactrls(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_aui_manager->AddPane(panel1, wxAuiPaneInfo().Name("Panel 1").Caption("Files").Left().CloseButton(true).MaximizeButton(true).MinSize(350, 200).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->AddPane(panel3, wxAuiPaneInfo().Name("Panel 3").Caption("Screen").Center().CloseButton(true).MaximizeButton(true).MinSize(200, 700).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->AddPane(m_panel_mediactrls, wxAuiPaneInfo().Name("panel_mediactrls").Caption("Controls").Center().CloseButton(true).MaximizeButton(true).MinSize(200, 50).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->AddPane(panel2, wxAuiPaneInfo().Name("Panel 4").Caption("Filters").Right().CloseButton(true).MaximizeButton(true).MinSize(350, 200).BestSize(300, 300).PaneBorder(false));
	
	m_aui_manager->Update();

	Layout();
}

gcr_frame_main::~gcr_frame_main()
{
}

void gcr_frame_main::set_window_settings_recursively(wxWindow* parent)
{
	if (dynamic_cast<wxPanel*>(parent))
	{
		parent->SetDoubleBuffered(true);
	}

	for (wxWindow* child : parent->GetChildren())
	{
		set_window_settings_recursively(child);
	}
}

void gcr_frame_main::menubar_exit_onclick(wxCommandEvent& event)
{
	wxTheApp->Exit();
}

void gcr_frame_main::menubar_settings_onclick(wxCommandEvent& event)
{
	m_frame_preferences = new gcr_frame_preferences("Preferences");
	wxIcon appIco("media/preferences16.ico", wxBITMAP_TYPE_ICO);
	m_frame_preferences->SetIcon(appIco);
	m_frame_preferences->SetInitialSize(wxWindow::FromDIP(wxSize(300, 600), 0));
	m_frame_preferences->Center();
	m_frame_preferences->Show();
}

void gcr_frame_main::menubar_about_onclick(wxCommandEvent& event)
{
	printf("About clicked\n");
}
