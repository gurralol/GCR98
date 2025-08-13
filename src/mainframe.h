#pragma once
#include <wx/wxprec.h>

#include "gui/menubar.h"
#include "gui/panel_master.h"

#include "utilities/filemanager.h"
#include "utilities/mediaplayer.h"
#include "utilities/filtermanager.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);

    // GUI
    wxBoxSizer* boxSizerFrame;
    PanelMaster* panelMaster;

    // Ptrs
    PanelFilesSettings* panelFilesSettings;
    PanelFiles* panelFiles;
    PanelPlaylist* panelPlaylist;
    PanelMedia* panelMedia;
    PanelMediaCtrls* panelMediaCtrls;
    PanelFiltersSettings* panelFiltersSettings;
    PanelFilters* panelFilters;
    PanelRenderSettings* panelRenderSettings;

    // MenuBar
    MenuBar* menuBar;

    // Utilities
    FileManager* fileManager;
    FileManagerExtended* fileManagerEx;
    MediaPlayer* mediaPlayer;
    FilterManager* filterManager;

    //Bindings
    void OpenFiles(wxCommandEvent& event);
    void AddFilter(wxCommandEvent& event);
    void FileClick(wxCommandEvent& event);
    void MediaPaint(wxPaintEvent& event);
    void OnResize(wxSizeEvent& event);

    void SliderPtsRefresh();
    
    void BtnBack(wxCommandEvent& event);
    void BtnPlayPause(wxCommandEvent& event);
    void BtnStop(wxCommandEvent& event);
    void BtnForward(wxCommandEvent& event);
    void BtnSeek(wxCommandEvent& event);
    void BtnLoop(wxCommandEvent& event);

    void SliderVolume(wxCommandEvent& event);
    void SliderPts(wxCommandEvent& event);

    // Functions
    void SetPanelSettingsRecursively(wxWindow* parent);

private:
    wxDECLARE_EVENT_TABLE();
};
