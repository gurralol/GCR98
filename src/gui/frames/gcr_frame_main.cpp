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

	// Media Player with callback
	m_media_player = new media_player([this](const unsigned char* rgb_data, int width, int height, int linesize) {
		this->update_media_screen();
	});

	// Main Sizer
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);

	// Panels
	m_panel_files = new gcr_panel_files(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_panel_playlist = new gcr_panel_playlist(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_panel_screen = new gcr_panel_screen(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_panel_mediactrls = new gcr_panel_mediactrls(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_panel_filters = new gcr_panel_filters(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_aui_manager->AddPane(m_panel_files, wxAuiPaneInfo().Name("panel_files").Caption("Files").Left().CloseButton(true).MaximizeButton(true).MinSize(350, 700).BestSize(300, 700).PaneBorder(false));
	m_aui_manager->AddPane(m_panel_playlist, wxAuiPaneInfo().Name("panel_playlist").Caption("Playlist").Left().CloseButton(true).MaximizeButton(true).MinSize(200, 150).BestSize(300, 150).PaneBorder(false));
	m_aui_manager->AddPane(m_panel_screen, wxAuiPaneInfo().Name("panel_screen").Caption("Screen").Center().CloseButton(true).MaximizeButton(true).MinSize(200, 700).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->AddPane(m_panel_mediactrls, wxAuiPaneInfo().Name("panel_mediactrls").Caption("Controls").Center().CloseButton(true).MaximizeButton(true).MinSize(200, 50).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->AddPane(m_panel_filters, wxAuiPaneInfo().Name("panel_filters").Caption("Filters").Right().CloseButton(true).MaximizeButton(true).MinSize(350, 200).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->Update();

	// Apply settings recursively
	set_window_settings_recursively(this);

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

void gcr_frame_main::menubar_open_directory_onclick(wxCommandEvent& event)
{
	wxDir dir;
	wxString dir_path = wxDirSelector("Select a directory to open", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
	if (dir_path.IsEmpty())
	{
		return;
	}

	m_file_manager->clear_files();
	
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

	populate_files_panel();

	event.Skip();
}

void gcr_frame_main::menubar_exit_onclick(wxCommandEvent& event)
{
	wxTheApp->Exit();
	event.Skip();
}

void gcr_frame_main::menubar_settings_onclick(wxCommandEvent& event)
{
	m_frame_preferences = new gcr_frame_preferences("Preferences");
	wxIcon appIco("media/preferences16.ico", wxBITMAP_TYPE_ICO);
	m_frame_preferences->SetIcon(appIco);
	m_frame_preferences->SetInitialSize(wxWindow::FromDIP(wxSize(300, 600), 0));
	m_frame_preferences->Center();
	m_frame_preferences->Show();
	event.Skip();
}

void gcr_frame_main::menubar_about_onclick(wxCommandEvent& event)
{
	printf("About clicked\n");
	event.Skip();
}

void gcr_frame_main::populate_files_panel()
{
	wxGridSizer* sizer = new wxGridSizer(0, 2, 0, 0);

	wxScrolledWindow* scrolled_window = m_panel_files->m_scrolled_window;

	if (scrolled_window->GetSizer())
	{
		scrolled_window->GetSizer()->Clear(true);
	}

	for (const auto& file_path : m_file_manager->m_files)
	{
		wxString file_name = file_path.filename().wstring();
		wxSize btn_size = wxSize(148, 128);
		gcr_button_thumbnail* file_button = new gcr_button_thumbnail(scrolled_window, wxID_ANY, "", wxDefaultPosition, btn_size);
		file_button->SetClientData(new std::filesystem::path(file_path));
		wxImage thumbnail = img_utils::get_thumbnail_cache(file_path, btn_size.x, btn_size.y, img_utils::KEEP_AR);
		if (thumbnail.IsOk())
		{
			wxBitmap bitmap(thumbnail);
			file_button->m_thumbnail_image = thumbnail;
		}
		file_button->on_left_up_cb = [this](const wxMouseEvent& event) { this->file_thumbnail_on_left_up(const_cast<wxMouseEvent&>(event)); };
		sizer->Add(file_button, 0);
	}

	scrolled_window->SetSizerAndFit(sizer);
	m_panel_files->Layout();
}

void gcr_frame_main::file_thumbnail_on_left_up(wxMouseEvent& event)
{
	gcr_button_base* button = dynamic_cast<gcr_button_base*>(event.GetEventObject());
	if (button)
	{
		std::filesystem::path* path_ptr = static_cast<std::filesystem::path*>(button->GetClientData());
		if (path_ptr)
		{
			wxString msg = path_ptr->wstring();
			printf("%s\n", msg.ToUTF8().data());
		}
	}
}

void gcr_frame_main::update_media_screen()
{
	// This will be called by the media player when a new frame is ready
	// You can update the screen panel here with the new frame data
	if (m_panel_screen)
	{
		m_panel_screen->Refresh();
	}
}
