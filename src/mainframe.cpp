#pragma once
#include <wx/wxprec.h>

#include <thread>
#include <atomic>

#include "mainframe.h"
#include "utilities/ids.h"

// test
#include "gui/panel_filter.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(gcrOPEN_FILE, MainFrame::MenuOpenFiles)
    EVT_MENU(gcrOPEN_DIRECTORY, MainFrame::MenuOpenDirectory)
    EVT_MENU(gcrEXIT, MainFrame::MenuExit)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxPoint(410, 200))
{
    wxInitAllImageHandlers();
    
    // GUI
    menuBar = new MenuBar();

    boxSizerMaster = new wxBoxSizer(wxHORIZONTAL);
    boxSizerLeft = new wxBoxSizer(wxVERTICAL);
    boxSizerMiddle = new wxBoxSizer(wxVERTICAL);
    boxSizerRight = new wxBoxSizer(wxVERTICAL);

    this->SetSizer(boxSizerMaster);

    panelFilesSettings = new PanelFilesSettings(this, wxID_ANY, wxDefaultPosition, wxSize(200, 100));
    panelFiles = new PanelFiles(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
    panelPlaylist = new PanelPlaylist(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
    panelMedia = new PanelMedia(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    panelMediaCtrls = new PanelMediaCtrls(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
    panelFiltersSettings = new PanelFiltersSettings(this, wxID_ANY, wxDefaultPosition, wxSize(200, 100));
    panelFilters = new PanelFilters(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));
    panelRenderSettings = new PanelRenderSettings(this, wxID_ANY, wxDefaultPosition, wxSize(200, 200));

    boxSizerMaster->Add(boxSizerLeft, 0, wxEXPAND);
    boxSizerMaster->Add(boxSizerMiddle, 1, wxEXPAND);
    boxSizerMaster->Add(boxSizerRight, 0, wxEXPAND);

    boxSizerLeft->Add(panelFilesSettings, 0);
    boxSizerLeft->Add(panelFiles, 1);
    boxSizerLeft->Add(panelPlaylist, 0);
    boxSizerMiddle->Add(panelMedia, 1, wxEXPAND);
    boxSizerMiddle->Add(panelMediaCtrls, 0, wxEXPAND);
    boxSizerRight->Add(panelFiltersSettings, 0);
    boxSizerRight->Add(panelFilters, 1);
    boxSizerRight->Add(panelRenderSettings, 0);

    SetMenuBar(menuBar);
    CreateStatusBar(1);

    // Filter adding test
    PanelFilter* panelFilterTest = new PanelFilter(panelFilters, wxID_ANY, wxDefaultPosition, wxSize(32, 200));
    panelFilters->boxSizerMaster->Add(panelFilterTest, 0, wxEXPAND);

    // Utilities
    gcrFileManager = new FileManager();
    gcrFileManagerExtended = new FileManagerExtended(*panelFiles, *panelPlaylist, *panelFilesSettings, *panelMediaCtrls);
    gcrMediaPlayer = new MediaPlayer();
    gcrFilterManager = new FilterManager();

    // Bindings
    // PanelFilesSettings
    panelFilesSettings->choiceRecursive->Bind(wxEVT_CHOICE, &MainFrame::SetRecursiveMode, this);
    panelFilesSettings->choiceSort->Bind(wxEVT_CHOICE, &MainFrame::SetSortMode, this);
    panelFilesSettings->choiceView->Bind(wxEVT_CHOICE, &MainFrame::SetViewMode, this);
    panelFilesSettings->choiceGroup->Bind(wxEVT_CHOICE, &MainFrame::SetGroupMode, this);

    SetPanelSettingsRecursively(this);
}

void MainFrame::MenuOpenFiles(wxCommandEvent& event)
{
    // Open file dialog
    wxFileDialog openFileDialog(this, "Select file(s)", "", "", "*.*", wxFD_OPEN | wxFD_MULTIPLE);
    openFileDialog.ShowModal();

    wxArrayString arrStr;
    openFileDialog.GetPaths(arrStr);

    if (arrStr.IsEmpty()) return;

    // gcrFileManager assignment
    gcrFileManager->directory = openFileDialog.GetDirectory().ToUTF8().data();

    gcrFileManager->filePaths.clear();
    gcrFileManagerExtended->filePaths.clear();

    for (const auto& path : arrStr)
    {
        if (gcrFileManager->IsSupportedFile(path.ToUTF8().data()) != true) continue;

        gcrFileManager->filePaths.push_back(path.ToUTF8().data());
        gcrFileManagerExtended->filePaths.push_back(path.ToUTF8().data());
    }

    gcrFileManager->Sort();

    gcrFileManagerExtended->arrBitmapButtons.clear();
    gcrFileManagerExtended->arrContentButtons.clear();

    gcrFileManagerExtended->RefreshAll();

    event.Skip();
}

void MainFrame::MenuOpenDirectory(wxCommandEvent& event)
{
    // Open directory dialog
    wxDir dirTemp = wxDirSelector();

    if (!dirTemp.Exists(dirTemp.GetName())) return;

    // gcrFileManager assignment
    gcrFileManager->directory = dirTemp.GetName().ToUTF8().data();

    gcrFileManager->filePaths.clear();

    gcrFileManager->OpenDirectory();

    gcrFileManager->Sort();

    gcrFileManagerExtended->filePaths = gcrFileManager->filePaths;
    gcrFileManagerExtended->directory = gcrFileManager->directory;

    gcrFileManagerExtended->arrBitmapButtons.clear();
    gcrFileManagerExtended->arrContentButtons.clear();

    gcrFileManagerExtended->RefreshAll();

    event.Skip();
}

void MainFrame::MenuExit(wxCommandEvent& event)
{
    this->Destroy();

    event.Skip();
}

// PanelFilesSettings
void MainFrame::SetRecursiveMode(wxCommandEvent& event)
{
    gcrFileManager->recursiveMode = event.GetSelection();

    gcrFileManagerExtended->RefreshAll();
}

void MainFrame::SetSortMode(wxCommandEvent& event)
{
    gcrFileManager->sortMode = event.GetSelection();
    gcrFileManagerExtended->sortMode = event.GetSelection();

    gcrFileManagerExtended->RefreshAll();
}

void MainFrame::SetViewMode(wxCommandEvent& event)
{
    gcrFileManagerExtended->viewMode = event.GetSelection();

    gcrFileManagerExtended->RefreshAll();
}

void MainFrame::SetGroupMode(wxCommandEvent& event)
{
    gcrFileManagerExtended->groupMode = event.GetSelection();

    gcrFileManagerExtended->RefreshAll();
}

void MainFrame::SetPanelSettingsRecursively(wxWindow* parent)
{
    if (dynamic_cast<wxPanel*>(parent))
    {
        parent->SetDoubleBuffered(true);
    }

    for (wxWindow* child : parent->GetChildren())
    {
        SetPanelSettingsRecursively(child);
    }
}
