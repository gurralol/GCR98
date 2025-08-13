#pragma once
#include <wx/wxprec.h>
#include <wx/dir.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <filesystem>
#include <thread>
#include <atomic>

#include "mainframe.h"
#include "utilities/ids.h"
#include "utilities/img_utils.h"

#include "filters/filters.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(gcrOPEN_DIRECTORY, OpenFiles)
    EVT_MENU(gcrOPEN_FILES, OpenFiles)

    EVT_MENU(gcrFILTER_LEVELS, AddFilter)
    EVT_MENU(gcrFILTER_BLUR, AddFilter)
    EVT_MENU(gcrFILTER_FACESWAP, AddFilter)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxPoint(410, 200))
{
    wxInitAllImageHandlers();
    
    // GUI
    boxSizerFrame = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizerFrame);

    panelMaster = new PanelMaster(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    boxSizerFrame->Add(panelMaster, 1, wxEXPAND);

    panelFilesSettings = panelMaster->panelFilesSettings;
    panelFiles = panelMaster->panelFiles;
    panelPlaylist = panelMaster->panelPlaylist;
    panelMedia = panelMaster->panelMedia;
    panelMediaCtrls = panelMaster->panelMediaCtrls;
    panelFiltersSettings = panelMaster->panelFiltersSettings;
    panelFilters = panelMaster->panelFilters;
    panelRenderSettings = panelMaster->panelRenderSettings;

    menuBar = new MenuBar();
    SetMenuBar(menuBar);

    CreateStatusBar(1);

    SetPanelSettingsRecursively(this);

    // Utilities
    fileManager = new FileManager();
    fileManagerEx = new FileManagerExtended();
    mediaPlayer = new MediaPlayer();
    filterManager = new FilterManager();

    // Bindings
    panelMedia->Bind(wxEVT_PAINT, &MainFrame::MediaPaint, this);
    Bind(wxEVT_SIZE, &MainFrame::OnResize, this);
    mediaPlayer->paintCallback = [this]() {
        panelMedia->Refresh();
        //SliderPtsRefresh();
        };

    panelMediaCtrls->btnPlayPause->Bind(wxEVT_BUTTON, &MainFrame::BtnPlayPause, this);
    panelMediaCtrls->btnStop->Bind(wxEVT_BUTTON, &MainFrame::BtnStop, this);
    panelMediaCtrls->btnLoop->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::BtnLoop, this);
    panelMediaCtrls->sliderVolume->Bind(wxEVT_SCROLL_THUMBTRACK, &MainFrame::SliderVolume, this);
    panelMediaCtrls->sliderPts->Bind(wxEVT_SCROLL_THUMBTRACK, &MainFrame::SliderPts, this);
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

void MainFrame::OpenFiles(wxCommandEvent& event)
{
    if (event.GetId() == gcrOPEN_FILES)
    {
        wxFileDialog fileDialog(this, "Select file(s)", "", "", "*.*", wxFD_OPEN | wxFD_MULTIPLE);
        fileDialog.ShowModal();

        wxArrayString arrStr;
        fileDialog.GetPaths(arrStr);
        if (arrStr.empty()) return;

        fileManager->filePaths.clear();

        for (int i = 0; i < arrStr.size(); i++)
        {
            std::filesystem::path filePath = arrStr[i].ToStdWstring();
            if (!fileManager->IsSupportedFile(filePath)) continue;
            fileManager->filePaths.push_back(filePath);
        }
    }
    else if (event.GetId() == gcrOPEN_DIRECTORY)
    {
        wxDir dir = wxDirSelector();
        if (!dir.IsOpened()) return;

        fileManager->filePaths.clear();

        wxString directory = dir.GetName();

        std::filesystem::path dirPath = directory.ToStdWstring();
        fileManager->directory = dirPath;
        fileManager->OpenDirectory();
    }

    if (panelFiles->scrolledWindowFiles->GetSizer())
    {
        panelFiles->scrolledWindowFiles->GetSizer()->Clear(true);
    }

    panelFiles->Freeze();

    std::vector<std::filesystem::path>& filePaths = fileManager->filePaths;
    wxGridSizer* gridSizer = new wxGridSizer(2, 0, 0);
    for (int i = 0; i < filePaths.size(); i++)
    {
        wxImage thumb;
        thumb = ImgUtils::GetThumbnail_ThumbnailCache(filePaths[i], 80, 51, ImgUtils::CENTER);

        wxButton* btn = new wxButton(panelFiles->scrolledWindowFiles, wxID_ANY, "", wxDefaultPosition, wxSize(84, 55));
        btn->SetBitmap(thumb);
        btn->SetClientData(new std::filesystem::path(filePaths[i]));
        btn->Bind(wxEVT_BUTTON, &MainFrame::FileClick, this);
        
        gridSizer->Add(btn, 0, wxALIGN_CENTER | wxBOTTOM, 1);
    }
    panelFiles->scrolledWindowFiles->SetSizer(gridSizer, true);
    panelFiles->Layout();

    panelFiles->Thaw();
}

void MainFrame::AddFilter(wxCommandEvent& event)
{
    this->Freeze();

    if (event.GetId() == gcrFILTER_LEVELS)
    {
    }
    else if (event.GetId() == gcrFILTER_BLUR)
    {
        FilterBlur* filter = new FilterBlur(panelFilters, wxID_ANY, wxDefaultPosition, wxSize(50, 200));
        filterManager->arrFilters.push_back(filter);
        panelFilters->boxSizerMaster->Add(filter, 0, wxEXPAND);
    }
    else if (event.GetId() == gcrFILTER_FACESWAP)
    {
        FilterFaceswap* filter = new FilterFaceswap(panelFilters, wxID_ANY, wxDefaultPosition);
        filter->currFrame = &mediaPlayer->currFrame;
        filterManager->arrFilters.push_back(filter);
        panelFilters->boxSizerMaster->Add(filter, 0, wxEXPAND);
    }

    panelFilters->Layout();

    this->Thaw();
    
    event.Skip();
}

void MainFrame::FileClick(wxCommandEvent& event)
{
    mediaPlayer->StopProcessing();

    if (panelMediaCtrls->btnAutoPlay->GetValue() == 1)
    {
        mediaPlayer->restrictProcessing = false;
    }
    else {}

    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    std::filesystem::path* filePath = static_cast<std::filesystem::path*>(btn->GetClientData());

    mediaPlayer->FreeAll();
    mediaPlayer->path = *filePath;

    mediaPlayer->seekTarget = 0;
    panelMediaCtrls->sliderPts->SetValue(0);

    if (filePath->extension() == ".webp")
    {
        mediaPlayer->InitWebP();
        std::thread([this]() {
            mediaPlayer->DecodeFrameLibwebp();
            }).detach();
    }
    else
    {
        mediaPlayer->InitFFmpeg();
        std::thread([this]() {
            mediaPlayer->DecodeFrameFFmpeg();
            }).detach();
    }
}

void MainFrame::MediaPaint(wxPaintEvent& event)
{
    if (!mediaPlayer->currFrame.IsOk()) return;

    wxImage safe;

    {
        std::lock_guard<std::mutex> lock(mediaPlayer->frameMutex);

        if (mediaPlayer->currFrame.IsOk() && mediaPlayer->currFrame.GetData() != nullptr)
        {
            safe = mediaPlayer->currFrame.Copy();
            
        }
    }

    for (auto& filter : filterManager->arrFilters)
    {
        if (filter->btnBypass->GetValue() == 1) continue;
        safe = filter->ApplyFilter(safe);
    }

    if (!safe.IsOk()) return;

    int panelSizeX = panelMedia->GetSize().x;
    int panelSizeY = panelMedia->GetSize().y;
    safe = ImgUtils::Resize_Fit(safe, panelSizeX, panelSizeY);
    wxPoint pos = ImgUtils::Position_Center(safe, panelSizeX, panelSizeY);

    wxAutoBufferedPaintDC dc(panelMaster->panelMedia);
    dc.Clear();
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    dc.DrawBitmap(safe, pos.x, pos.y, false);

    delete gc;
    event.Skip();
}

void MainFrame::SliderPtsRefresh()
{
    int sliderVal = (mediaPlayer->currTime / mediaPlayer->duration) * 1000;
    panelMediaCtrls->sliderPts->SetValue(sliderVal);
}

void MainFrame::OnResize(wxSizeEvent& event)
{
    panelMedia->Refresh();
    event.Skip();
}

void MainFrame::BtnPlayPause(wxCommandEvent& event)
{
    if (!mediaPlayer->isProcessing)
    {
        mediaPlayer->StopProcessing();
        mediaPlayer->restrictProcessing = false;

        int sliderVal = panelMediaCtrls->sliderPts->GetValue();
        mediaPlayer->seekTarget = (static_cast<double>(sliderVal) / 1000) * mediaPlayer->duration;
        mediaPlayer->SeekFFmpeg();

        std::thread([this]() {
            mediaPlayer->DecodeFrameFFmpeg();
            }).detach();
    }
    else if (mediaPlayer->isProcessing)
    {
        mediaPlayer->StopProcessing();
    }

    event.Skip();
}

void MainFrame::BtnStop(wxCommandEvent& event)
{
    mediaPlayer->StopProcessing();
    
    event.Skip();
}

void MainFrame::BtnSeek(wxCommandEvent& event)
{
    event.Skip();
}

void MainFrame::BtnLoop(wxCommandEvent& event)
{
    int val = panelMediaCtrls->btnLoopVal;

    mediaPlayer->loop = false;

    if (val == 0) // Loop
    {
    }
    else if (val == 1) // Loop one
    {
        mediaPlayer->loop = true;
    }
    else if (val == 2) // Shuffle
    {
    }
    else // Disabled
    {
    }

    event.Skip();
}

void MainFrame::SliderVolume(wxCommandEvent& event)
{
    int sliderVal = panelMediaCtrls->sliderVolume->GetValue();
    mediaPlayer->volume = static_cast<float>(sliderVal) / 20;
}

void MainFrame::SliderPts(wxCommandEvent& event)
{
    if (mediaPlayer->isProcessing)
    {
        mediaPlayer->StopProcessing();
    }

    if (mediaPlayer->path.extension() == ".webp")
    {

    }
    else
    {
        int sliderVal = panelMediaCtrls->sliderPts->GetValue();
        mediaPlayer->seekTarget = (static_cast<double>(sliderVal) / 1000) * mediaPlayer->duration;
        mediaPlayer->SeekFFmpeg();

        std::thread([this](){
            mediaPlayer->DecodeFrameFFmpeg();
            }).detach();
    }

    event.Skip();
}
