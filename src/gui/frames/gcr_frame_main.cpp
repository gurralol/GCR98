#pragma once
#include "gcr_frame_main.h"

#include "../../utilities/theme_manager.h"
#include "../buttons/gcr_button_thumbnail.h"

wxBEGIN_EVENT_TABLE(gcr_frame_main, wxFrame)
	EVT_MENU(gcr_OPEN_DIRECTORY, menubar_open_directory_onclick)
	EVT_MENU(gcr_EXIT, menubar_exit_onclick)
	EVT_MENU(gcr_PREFERENCES, menubar_settings_onclick)
	EVT_MENU(gcr_ABOUT, menubar_about_onclick)
wxEND_EVENT_TABLE()

gcr_frame_main::gcr_frame_main(const wxString& title) : gcr_frame_base(title)
{
	// Initialize Image Handlers
    wxInitAllImageHandlers();

	// Set Colours
	this->SetBackgroundColour(wxColour(0, 0, 0));
	this->SetForegroundColour(wxColour(0, 0, 0));

	// Menu Bar
	m_menubar = new gcr_menubar_mainframe();
	SetMenuBar(m_menubar);

	// AUI Manager
	m_aui_manager = new gcr_aui_manager(this, wxAUI_MGR_DEFAULT | wxAUI_MGR_LIVE_RESIZE);

	// File Manager
	m_file_manager = new file_manager();

	// Main Sizer
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);

	// Panels
	m_panel_files = new gcr_panel_files(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_panel_screen = new gcr_panel_screen(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_panel_mediactrls = new gcr_panel_mediactrls(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_panel_filters = new gcr_panel_filters(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_aui_manager->AddPane(m_panel_files, wxAuiPaneInfo().Name("panel_files").Caption("Files").Left().CloseButton(true).MaximizeButton(true).MinSize(350, 200).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->AddPane(m_panel_screen, wxAuiPaneInfo().Name("panel_screen").Caption("Screen").Center().CloseButton(true).MaximizeButton(true).MinSize(200, 700).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->AddPane(m_panel_mediactrls, wxAuiPaneInfo().Name("panel_mediactrls").Caption("Controls").Center().CloseButton(true).MaximizeButton(true).MinSize(200, 50).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->AddPane(m_panel_filters, wxAuiPaneInfo().Name("panel_filters").Caption("Filters").Right().CloseButton(true).MaximizeButton(true).MinSize(350, 200).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->Update();

	// Apply settings recursively
	set_window_settings_recursively(this);

	Layout();
}

void add_file_buttons_to_panel(wxPanel* panel, file_manager* fm);

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

void gcr_frame_main::menubar_open_directory_onclick(wxCommandEvent& event)
{
	wxDir dir;
	wxString dir_path = wxDirSelector("Select a directory to open", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dir_path.IsEmpty())
	{
		return;
	}

	std::filesystem::path path(dir_path.ToStdWstring());
	if (m_file_manager->set_directory(path) != 0)
	{
		wxMessageBox("The selected path is not a valid directory.", "Error", wxOK | wxICON_ERROR);
		return;
	}

	if (m_file_manager->open_directory(true) != 0)
	{
		wxMessageBox("Failed to open the selected directory.", "Error", wxOK | wxICON_ERROR);
		return;
	}

	add_file_buttons_to_files_panel();

	event.Skip();
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

void gcr_frame_main::add_file_buttons_to_files_panel()
{
	wxGridSizer* sizer = m_panel_files->m_files_sizer;
	sizer->Clear(true);

	for (const auto& file_path : m_file_manager->m_files)
	{
		wxString file_name = file_path.filename().wstring();
		wxSize btn_size = wxSize(148, 128);
		gcr_button_thumbnail* file_button = new gcr_button_thumbnail(m_panel_files, wxID_ANY, "", wxDefaultPosition, btn_size);
		file_button->SetBackgroundColour(*wxBLACK);
		file_button->SetClientData(new std::filesystem::path(file_path));
		wxImage thumbnail = img_utils::get_thumbnail_cache(file_path, btn_size.x, btn_size.y, img_utils::KEEP_AR);
		if (thumbnail.IsOk())
		{
			wxBitmap bitmap(thumbnail);
			file_button->m_thumbnail_image = thumbnail;
		}
		sizer->Add(file_button, 0, wxALIGN_CENTER, 0);
	}
	m_panel_files->Layout();
}