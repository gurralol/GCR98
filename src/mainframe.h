#pragma once
#include <wx/wxprec.h>

// GUI
#include "gui/menubar.h"
#include "gui/panel_files_settings.h"
#include "gui/panel_files.h"
#include "gui/panel_playlist.h"
#include "gui/panel_media.h"
#include "gui/panel_mediactrls.h"
#include "gui/panel_filters_settings.h"
#include "gui/panel_filters.h"
#include "gui/panel_render_settings.h"

// Utilities
#include "utilities/filemanager.h"
#include "utilities/mediaplayer.h"
#include "utilities/filtermanager.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);

    // GUI
    wxBoxSizer* boxSizerMaster;
    wxBoxSizer* boxSizerLeft;
    wxBoxSizer* boxSizerMiddle;
    wxBoxSizer* boxSizerRight;

    MenuBar* menuBar;
    PanelFiles* panelFiles;
    PanelPlaylist* panelPlaylist;
    PanelMedia* panelMedia;
    PanelMediaCtrls* panelMediaCtrls;
    PanelFilters* panelFilters;
    PanelRenderSettings* panelRenderSettings;

    PanelFilesSettings* panelFilesSettings;
    PanelFiltersSettings* panelFiltersSettings;

    // Utilities
    FileManager* gcrFileManager;
    FileManagerExtended* gcrFileManagerExtended;
    MediaPlayer* gcrMediaPlayer;
    FilterManager* gcrFilterManager;

    void MenuOpenFiles(wxCommandEvent& event);
    void MenuOpenDirectory(wxCommandEvent& event);
    void MenuExit(wxCommandEvent& event);

    // PanelFilesSettings
    void SetRecursiveMode(wxCommandEvent& event);
    void SetSortMode(wxCommandEvent& event);
    void SetViewMode(wxCommandEvent& event);
    void SetGroupMode(wxCommandEvent& event);

    void SetPanelSettingsRecursively(wxWindow* parent);

private:
    wxDECLARE_EVENT_TABLE();
};
