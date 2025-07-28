#include <chrono>

#include "filemanager.h"

//----------------//
//  File Manager  // 
//----------------//

FileManager::FileManager()
{
    recursiveMode = 0;
    sortMode = 0;
}

FileManager::~FileManager()
{

}

int FileManager::OpenDirectory()
{
    if (recursiveMode == 1)
    {
        for (const auto& dir : std::filesystem::recursive_directory_iterator(directory))
        {
            if (!dir.is_directory())
            {
                if (!IsSupportedFile(dir.path())) continue;
                filePaths.push_back(dir.path());
            }
        }
        return 0;
    }

    for (const auto& dir : std::filesystem::directory_iterator(directory))
    {
        if (!dir.is_directory())
        {
            if (!IsSupportedFile(dir.path())) continue;
            filePaths.push_back(dir.path());
        }
    }

    return 0;
}

int FileManager::Sort()
{
    if (filePaths.empty()) return -1;

    if (sortMode == SORT_NAME)
    {
        std::sort(filePaths.begin(), filePaths.end());
        return 0;
    }
    else if (sortMode == SORT_DATE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                auto ftime_a = std::filesystem::last_write_time(a);
                auto ftime_b = std::filesystem::last_write_time(b);
                return ftime_a < ftime_b;
            });
        return 0;
    }
    else if (sortMode == SORT_SIZE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                return std::filesystem::file_size(a) < std::filesystem::file_size(b);
            });
        return 0;
    }
    else if (sortMode == SORT_TYPE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                return std::filesystem::path(a).extension() < std::filesystem::path(b).extension();
            });
        return 0;
    }
    else {}

    return 1;
}

bool FileManager::IsSupportedFile(std::filesystem::path path)
{
    std::string ext = path.extension().string().c_str();

    // Video formats
    if (ext == ".mp4") return true;
    else if (ext == ".webm") return true;
    else if (ext == ".avi") return true;
    else if (ext == ".mov") return true;
    else if (ext == ".mkv") return true;
    else if (ext == ".m4v") return true;
    else if (ext == ".mpeg") return true;

    // Image formats
    else if (ext == ".gif") return true;
    else if (ext == ".webp") return true;
    else if (ext == ".png") return true;
    else if (ext == ".jpg") return true;
    else if (ext == ".jpeg") return true;
    else if (ext == ".bmp") return true;

    return false;
}

//-------------------------//
//  File Manager Extended  // 
//-------------------------//

FileManagerExtended::FileManagerExtended(PanelFiles& panelFiles,
                                         PanelPlaylist& panelPlaylist,
                                         PanelFilesSettings& panelFilesSettings,
                                         PanelMediaCtrls& panelMediaCtrls) :
                                         panelFiles(panelFiles),
                                         panelPlaylist(panelPlaylist),
                                         panelFilesSettings(panelFilesSettings),
                                         panelMediaCtrls(panelMediaCtrls)
{
    sortMode = 0;
    viewMode = 0;
    groupMode = 0;

    wxUSE_FILECTRL;
}

FileManagerExtended::~FileManagerExtended()
{
}

void FileManagerExtended::RefreshAll()
{
    panelFiles.Freeze();

    if (viewMode == VIEW_GRID)
    {
        if (arrBitmapButtons.empty())
        {
            CreateBitmapButtons();
        }
        SortBitmapButtons();
    }
    else if (viewMode == VIEW_CONTENT)
    {
        if (arrContentButtons.empty())
        {
            CreateContentButtons();
        }
        SortContentButtons();
    }
    else if (viewMode == VIEW_DETAIL)
    {
        SortFilePaths();
    }
    else if (viewMode == VIEW_TREE) {}
    else {}
    
    ViewModeRefresh();

    panelFiles.Layout();
    panelFiles.Thaw();
}

void FileManagerExtended::ViewModeRefresh()
{
    // Detach buttons from their sizers
    for (int i = 0; i < arrBitmapButtons.size(); i++)
    {
        if (arrBitmapButtons[i]->GetContainingSizer() == nullptr) continue;
        arrBitmapButtons[i]->GetContainingSizer()->Detach(arrBitmapButtons[i]);
    }

    for (int i = 0; i < arrContentButtons.size(); i++)
    {
        if (arrContentButtons[i]->GetContainingSizer() == nullptr) continue;
        arrContentButtons[i]->GetContainingSizer()->Detach(arrContentButtons[i]);
    }

    // Reparent buttons so they dont get deleted when clearing sizer
    for (auto& btn : arrBitmapButtons)
    {
        btn->Reparent(panelFiles.scrolledWindowFiles);
    }
    for (auto& btn : arrContentButtons)
    {
        btn->Reparent(panelFiles.scrolledWindowFiles);
    }

    // Clear the children of the sizer
    if (panelFiles.scrolledWindowFiles->GetSizer() != nullptr)
    {
        panelFiles.scrolledWindowFiles->GetSizer()->Clear(true);
    }

    // Have fun!
    if (viewMode == VIEW_GRID) {
        panelFilesSettings.choiceRecursive->Enable();
        panelFilesSettings.choiceSort->Enable();
        panelFilesSettings.choiceGroup->Enable();
        panelFilesSettings.choiceView->Enable();

        for (const auto& btn : arrBitmapButtons)
        {
            btn->Show();
        }

        for (const auto& btn : arrContentButtons)
        {
            btn->Hide();
        }

        if (groupMode == GROUP_NONE)
        {
            wxGridSizer* gridSizer = new wxGridSizer(2, 0, 0);
            for (auto& btn : arrBitmapButtons)
            {
                btn->Reparent(panelFiles.scrolledWindowFiles);
                gridSizer->Add(btn, 0, wxALIGN_CENTER | wxBOTTOM, 1);
            }
            panelFiles.scrolledWindowFiles->SetSizer(gridSizer, true);
        }
        else if (groupMode == GROUP_TYPE)
        {
            wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
            panelFiles.scrolledWindowFiles->SetSizer(boxSizer, true);

            // What extension types exists in arrBitmapButtons?
            std::vector<std::string> extTypes;
            bool exists;
            for (const auto& btn : arrBitmapButtons)
            {
                auto* path = static_cast<std::filesystem::path*>(btn->GetClientData());
                exists = false;
                for (const auto& ext : extTypes)
                {
                    if (ext == path->extension().string()) exists = true;
                    if (exists) break;
                }
                if (!exists)
                {
                    extTypes.push_back(path->extension().string());
                }
            }

            std::sort(extTypes.begin(), extTypes.end());

            std::vector<wxGenericCollapsiblePane*> colPanes;

            // Create a collapsible pane for each extension
            for (const auto& ext : extTypes)
            {
                wxGenericCollapsiblePane* colPane = new wxGenericCollapsiblePane(panelFiles.scrolledWindowFiles, wxID_ANY, ext, wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
                colPane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this, colPane](wxCollapsiblePaneEvent& event)
                    {
                        panelFiles.Freeze();
                        colPane->GetPane()->Fit();
                        this->panelFiles.Layout();
                        this->panelFiles.Refresh();
                        panelFiles.Thaw();
                        event.Skip();
                    });
                wxGridSizer* gridSizer = new wxGridSizer(2, 0, 0);
                colPane->GetPane()->SetSizer(gridSizer);
                colPanes.push_back(colPane);
            }

            // Add button to the right pane
            for (const auto& btn : arrBitmapButtons)
            {
                for (const auto& pane : colPanes)
                {
                    auto* path = static_cast<std::filesystem::path*>(btn->GetClientData());
                    if (path->extension().string() == pane->GetLabelText().ToStdString())
                    {
                        btn->Reparent(pane->GetPane());
                        pane->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                    }
                }
            }

            for (const auto& pane : colPanes)
            {
                boxSizer->Add(pane, 0, wxEXPAND);
                pane->Expand();

                wxSize sizeCurr = pane->GetPane()->GetSize();
                sizeCurr = wxSize(0, sizeCurr.y + 8);
                pane->GetPane()->SetMinSize(sizeCurr);
            }
        }
        else if (groupMode == GROUP_DATE)
        {
            std::vector<std::string> dateRanges;
            dateRanges.push_back("Today");
            dateRanges.push_back("Yesterday");
            dateRanges.push_back("Last week");
            dateRanges.push_back("Last month");
            dateRanges.push_back("Last 6 months");
            dateRanges.push_back("Last year");
            dateRanges.push_back("Before");

            wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
            panelFiles.scrolledWindowFiles->SetSizer(boxSizer, true);

            std::vector<wxGenericCollapsiblePane*> colPanes;

            for (const auto& range : dateRanges)
            {
                wxGenericCollapsiblePane* colPane = new wxGenericCollapsiblePane(panelFiles.scrolledWindowFiles, wxID_ANY, range, wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
                colPane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this, colPane](wxCollapsiblePaneEvent& event)
                    {
                        panelFiles.Freeze();
                        colPane->GetPane()->Fit();
                        this->panelFiles.Layout();
                        this->panelFiles.Refresh();
                        panelFiles.Thaw();
                        event.Skip();
                    });
                wxGridSizer* gridSizer = new wxGridSizer(2, 0, 0);
                colPane->GetPane()->SetSizer(gridSizer);
                colPanes.push_back(colPane);
            }

            for (const auto& btn : arrBitmapButtons)
            {
                std::filesystem::path* path = static_cast<std::filesystem::path*>(btn->GetClientData());
                auto writeTime = std::filesystem::last_write_time(*path);

                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    writeTime - decltype(writeTime)::clock::now() + std::chrono::system_clock::now()
                );

                auto now = std::chrono::system_clock::now();
                auto age = std::chrono::duration_cast<std::chrono::hours>(now - sctp).count();

                if (age < 24) // Today
                {
                    btn->Reparent(colPanes[0]->GetPane());
                    colPanes[0]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (age < 48) // Yesterday
                {
                    btn->Reparent(colPanes[1]->GetPane());
                    colPanes[1]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (age < 24 * 7) // Last week
                {
                    btn->Reparent(colPanes[2]->GetPane());
                    colPanes[2]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (age < 24 * 30) // Last month
                {
                    btn->Reparent(colPanes[3]->GetPane());
                    colPanes[3]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (age < 24 * 180) // Last 6 months
                {
                    btn->Reparent(colPanes[4]->GetPane());
                    colPanes[4]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (age < 24 * 360) // Last year
                {
                    btn->Reparent(colPanes[5]->GetPane());
                    colPanes[5]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else // Before
                {
                    btn->Reparent(colPanes[6]->GetPane());
                    colPanes[6]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
            }

            for (const auto& pane : colPanes)
            {
                if (pane->GetPane()->GetSizer()->GetChildren().IsEmpty())
                {
                    boxSizer->Detach(pane);
                    pane->Destroy();
                    continue;
                }

                boxSizer->Add(pane, 0, wxEXPAND);

                pane->Expand();

                wxSize sizeCurr = pane->GetPane()->GetSize();
                sizeCurr = wxSize(0, sizeCurr.y + 8);
                pane->GetPane()->SetMinSize(sizeCurr);
            }
        }
        else if (groupMode == GROUP_SIZE)
        {
            std::vector<std::string> sizeRanges;
            sizeRanges.push_back("Miniscule: 0-1mb");
            sizeRanges.push_back("Tiny: 1-5mb");
            sizeRanges.push_back("Small: 5-25mb");
            sizeRanges.push_back("Medium: 25-100mb");
            sizeRanges.push_back("Large: 100-1.000mb");
            sizeRanges.push_back("Huge: 1-10gb");
            sizeRanges.push_back("Gigantic: 10-100gb");

            wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
            panelFiles.scrolledWindowFiles->SetSizer(boxSizer, true);

            std::vector<wxGenericCollapsiblePane*> colPanes;

            for (const auto& range : sizeRanges)
            {
                wxGenericCollapsiblePane* colPane = new wxGenericCollapsiblePane(panelFiles.scrolledWindowFiles, wxID_ANY, range, wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
                colPane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this, colPane](wxCollapsiblePaneEvent& event)
                    {
                        panelFiles.Freeze();
                        colPane->GetPane()->Fit();
                        this->panelFiles.Layout();
                        this->panelFiles.Refresh();
                        panelFiles.Thaw();
                        event.Skip();
                    });
                wxGridSizer* gridSizer = new wxGridSizer(2, 0, 0);
                colPane->GetPane()->SetSizer(gridSizer);
                colPanes.push_back(colPane);
            }

            for (const auto& btn : arrBitmapButtons)
            {
                std::filesystem::path* path = static_cast<std::filesystem::path*>(btn->GetClientData());

                auto fileSize = std::filesystem::file_size(*path);
                double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                if (fileSizeMb < 1.0) // Miniscule
                {
                    btn->Reparent(colPanes[0]->GetPane());
                    colPanes[0]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (fileSizeMb < 5.0) // Tiny
                {
                    btn->Reparent(colPanes[1]->GetPane());
                    colPanes[1]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (fileSizeMb < 25.0) // Small
                {
                    btn->Reparent(colPanes[2]->GetPane());
                    colPanes[2]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (fileSizeMb < 100.0) // Medium
                {
                    btn->Reparent(colPanes[3]->GetPane());
                    colPanes[3]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (fileSizeMb < 1000.0) // Large
                {
                    btn->Reparent(colPanes[4]->GetPane());
                    colPanes[4]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else if (fileSizeMb < 10000.0) // Huge
                {
                    btn->Reparent(colPanes[5]->GetPane());
                    colPanes[5]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
                else // Gigantic
                {
                    btn->Reparent(colPanes[6]->GetPane());
                    colPanes[6]->GetPane()->GetSizer()->Add(btn, 0, wxALIGN_CENTER_HORIZONTAL);
                }
            }

            for (const auto& pane : colPanes)
            {
                if (pane->GetPane()->GetSizer()->GetChildren().IsEmpty())
                {
                    boxSizer->Detach(pane);
                    pane->Destroy();
                    continue;
                }

                boxSizer->Add(pane, 0, wxEXPAND);

                pane->Expand();

                wxSize sizeCurr = pane->GetPane()->GetSize();
                sizeCurr = wxSize(0, sizeCurr.y + 8);
                pane->GetPane()->SetMinSize(sizeCurr);
            }
        }
        else {}
    }
    else if (viewMode == VIEW_CONTENT) {
        panelFilesSettings.choiceRecursive->Enable();
        panelFilesSettings.choiceSort->Enable();
        panelFilesSettings.choiceGroup->Enable();
        panelFilesSettings.choiceView->Enable();

        for (const auto& btn : arrBitmapButtons)
        {
            btn->Hide();
        }

        for (const auto& btn : arrContentButtons)
        {
            btn->Show();
        }

        if (groupMode == GROUP_NONE)
        {
            wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
            panelFiles.scrolledWindowFiles->SetSizer(boxSizer, true);

            for (const auto& btn : arrContentButtons)
            {
                wxBoxSizer* sizerTile = new wxBoxSizer(wxHORIZONTAL);
                wxPanel* panel = new wxPanel(panelFiles.scrolledWindowFiles, wxID_ANY, wxDefaultPosition, wxSize(48, 44), wxBORDER_SIMPLE);
                panel->SetBackgroundColour(wxColor(232, 232, 232));
                panel->SetSizer(sizerTile);

                panel->Bind(wxEVT_LEFT_DOWN, [panel](wxMouseEvent& event)
                    {
                        printf("Click\n");
                        event.Skip();
                    });

                panel->Bind(wxEVT_ENTER_WINDOW, [panel](wxMouseEvent& event)
                    {
                        panel->SetBackgroundColour(wxColor(255, 255, 255));
                        panel->Refresh();
                        event.Skip();
                    });

                panel->Bind(wxEVT_LEAVE_WINDOW, [panel](wxMouseEvent& event)
                    {
                        panel->SetBackgroundColour(wxColor(232, 232, 232));
                        panel->Refresh();
                        event.Skip();
                    });

                btn->Reparent(panel);
                sizerTile->Add(btn, 0);

                std::filesystem::path* path = static_cast<std::filesystem::path*>(btn->GetClientData());

                auto fileSize = std::filesystem::file_size(*path);
                double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                wxString label(path->filename().wstring().c_str(), wxConvUTF8);

                wxString labelSize(wxString::Format("%.2f mb", fileSizeMb));

                wxStaticText* textName = new wxStaticText(panel, wxID_ANY, label, wxDefaultPosition, wxDefaultSize);
                wxStaticText* textSize = new wxStaticText(panel, wxID_ANY, labelSize, wxDefaultPosition, wxDefaultSize);

                wxBoxSizer* boxSizerText = new wxBoxSizer(wxVERTICAL);

                boxSizerText->Add(textName, 0);
                boxSizerText->Add(textSize, 0);

                sizerTile->Add(boxSizerText, 0, wxALIGN_CENTER);

                boxSizer->Add(panel, 0, wxEXPAND | wxBOTTOM, 3);
            }
        }
        else if (groupMode == GROUP_TYPE)
        {
            wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
            panelFiles.scrolledWindowFiles->SetSizer(boxSizer, true);

            // What extension types exists in arrBitmapButtons?
            std::vector<std::string> extTypes;
            bool exists;
            for (const auto& btn : arrContentButtons)
            {
                auto* path = static_cast<std::filesystem::path*>(btn->GetClientData());
                exists = false;
                for (const auto& ext : extTypes)
                {
                    if (ext == path->extension().string()) exists = true;
                    if (exists) break;
                }
                if (!exists)
                {
                    extTypes.push_back(path->extension().string());
                }
            }

            std::sort(extTypes.begin(), extTypes.end());

            std::vector<wxGenericCollapsiblePane*> colPanes;

            // Create a collapsible pane for each extension
            for (const auto& ext : extTypes)
            {
                wxGenericCollapsiblePane* colPane = new wxGenericCollapsiblePane(panelFiles.scrolledWindowFiles, wxID_ANY, ext, wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
                colPane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this, colPane](wxCollapsiblePaneEvent& event)
                    {
                        panelFiles.Freeze();
                        colPane->GetPane()->Fit();
                        this->panelFiles.Layout();
                        this->panelFiles.Refresh();
                        panelFiles.Thaw();
                        event.Skip();
                    });
                wxGridSizer* gridSizer = new wxGridSizer(1, 0, 0);
                colPane->GetPane()->SetSizer(gridSizer);
                colPanes.push_back(colPane);
            }

            // Add button to the right pane
            for (const auto& btn : arrContentButtons)
            {
                for (const auto& pane : colPanes)
                {
                    auto* path = static_cast<std::filesystem::path*>(btn->GetClientData());
                    if (path->extension().string() == pane->GetLabelText().ToStdString())
                    {
                        wxBoxSizer* sizerTile = new wxBoxSizer(wxHORIZONTAL);
                        wxPanel* panel = new wxPanel(pane->GetPane(), wxID_ANY, wxDefaultPosition, wxSize(48, 44), wxBORDER_SIMPLE);
                        panel->SetBackgroundColour(wxColor(232, 232, 232));
                        panel->SetSizer(sizerTile);

                        panel->Bind(wxEVT_LEFT_DOWN, [panel](wxMouseEvent& event)
                            {
                                printf("Click\n");
                                event.Skip();
                            });

                        panel->Bind(wxEVT_ENTER_WINDOW, [panel](wxMouseEvent& event)
                            {
                                panel->SetBackgroundColour(wxColor(255, 255, 255));
                                panel->Refresh();
                                event.Skip();
                            });

                        panel->Bind(wxEVT_LEAVE_WINDOW, [panel](wxMouseEvent& event)
                            {
                                panel->SetBackgroundColour(wxColor(232, 232, 232));
                                panel->Refresh();
                                event.Skip();
                            });

                        btn->Reparent(panel);
                        sizerTile->Add(btn, 0);

                        auto fileSize = std::filesystem::file_size(*path);
                        double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                        wxString label(path->filename().wstring().c_str(), wxConvUTF8);
                        wxString labelSize(wxString::Format("%.2f mb", fileSizeMb));

                        wxStaticText* textName = new wxStaticText(panel, wxID_ANY, label, wxDefaultPosition, wxDefaultSize);
                        wxStaticText* textSize = new wxStaticText(panel, wxID_ANY, labelSize, wxDefaultPosition, wxDefaultSize);

                        wxBoxSizer* boxSizerText = new wxBoxSizer(wxVERTICAL);

                        boxSizerText->Add(textName, 0);
                        boxSizerText->Add(textSize, 0);

                        sizerTile->Add(boxSizerText, 0, wxALIGN_CENTER);

                        pane->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                    }
                }
            }

            for (const auto& pane : colPanes)
            {
                boxSizer->Add(pane, 0, wxEXPAND);
                pane->Expand();
            }
        }
        else if (groupMode == GROUP_DATE)
        {
            std::vector<std::string> dateRanges;
            dateRanges.push_back("Today");
            dateRanges.push_back("Yesterday");
            dateRanges.push_back("Last week");
            dateRanges.push_back("Last month");
            dateRanges.push_back("Last 6 months");
            dateRanges.push_back("Last year");
            dateRanges.push_back("Before");

            wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
            panelFiles.scrolledWindowFiles->SetSizer(boxSizer, true);

            std::vector<wxGenericCollapsiblePane*> colPanes;

            for (const auto& range : dateRanges)
            {
                wxGenericCollapsiblePane* colPane = new wxGenericCollapsiblePane(panelFiles.scrolledWindowFiles, wxID_ANY, range, wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
                colPane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this, colPane](wxCollapsiblePaneEvent& event)
                    {
                        panelFiles.Freeze();
                        colPane->GetPane()->Fit();
                        this->panelFiles.Layout();
                        this->panelFiles.Refresh();
                        panelFiles.Thaw();
                        event.Skip();
                    });
                wxGridSizer* gridSizer = new wxGridSizer(1, 0, 0);
                colPane->GetPane()->SetSizer(gridSizer);
                colPanes.push_back(colPane);
            }

            for (const auto& btn : arrContentButtons)
            {
                std::filesystem::path* path = static_cast<std::filesystem::path*>(btn->GetClientData());
                auto writeTime = std::filesystem::last_write_time(*path);

                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    writeTime - decltype(writeTime)::clock::now() + std::chrono::system_clock::now()
                );

                auto now = std::chrono::system_clock::now();
                auto age = std::chrono::duration_cast<std::chrono::hours>(now - sctp).count();

                wxBoxSizer* sizerTile = new wxBoxSizer(wxHORIZONTAL);
                wxPanel* panel = new wxPanel(colPanes[0]->GetPane(), wxID_ANY, wxDefaultPosition, wxSize(48, 44), wxBORDER_SIMPLE);
                panel->SetBackgroundColour(wxColor(232, 232, 232));
                panel->SetSizer(sizerTile);

                panel->Bind(wxEVT_LEFT_DOWN, [panel](wxMouseEvent& event)
                    {
                        printf("Click\n");
                        event.Skip();
                    });

                panel->Bind(wxEVT_ENTER_WINDOW, [panel](wxMouseEvent& event)
                    {
                        panel->SetBackgroundColour(wxColor(255, 255, 255));
                        panel->Refresh();
                        event.Skip();
                    });

                panel->Bind(wxEVT_LEAVE_WINDOW, [panel](wxMouseEvent& event)
                    {
                        panel->SetBackgroundColour(wxColor(232, 232, 232));
                        panel->Refresh();
                        event.Skip();
                    });

                btn->Reparent(panel);
                sizerTile->Add(btn, 0);

                auto fileSize = std::filesystem::file_size(*path);
                double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                wxString label(path->filename().wstring().c_str(), wxConvUTF8);
                wxString labelSize(wxString::Format("%.2f mb", fileSizeMb));

                wxStaticText* textName = new wxStaticText(panel, wxID_ANY, label, wxDefaultPosition, wxDefaultSize);
                wxStaticText* textSize = new wxStaticText(panel, wxID_ANY, labelSize, wxDefaultPosition, wxDefaultSize);

                wxBoxSizer* boxSizerText = new wxBoxSizer(wxVERTICAL);

                boxSizerText->Add(textName, 0);
                boxSizerText->Add(textSize, 0);

                sizerTile->Add(boxSizerText, 0, wxALIGN_CENTER);

                if (age < 24) // Today
                {
                    panel->Reparent(colPanes[0]->GetPane());
                    colPanes[0]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (age < 48) // Yesterday
                {
                    panel->Reparent(colPanes[1]->GetPane());
                    colPanes[1]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (age < 24 * 7) // Last week
                {
                    panel->Reparent(colPanes[2]->GetPane());
                    colPanes[2]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (age < 24 * 30) // Last month
                {
                    panel->Reparent(colPanes[3]->GetPane());
                    colPanes[3]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (age < 24 * 180) // Last 6 months
                {
                    panel->Reparent(colPanes[4]->GetPane());
                    colPanes[4]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (age < 24 * 360) // Last year
                {
                    panel->Reparent(colPanes[5]->GetPane());
                    colPanes[5]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else // Before
                {
                    panel->Reparent(colPanes[6]->GetPane());
                    colPanes[6]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
            }

            for (const auto& pane : colPanes)
            {
                if (pane->GetPane()->GetSizer()->GetChildren().IsEmpty())
                {
                    boxSizer->Detach(pane);
                    pane->Destroy();
                    continue;
                }

                boxSizer->Add(pane, 0, wxEXPAND);

                pane->Expand();
            }
        }
        else if (groupMode == GROUP_SIZE)
        {
            std::vector<std::string> sizeRanges;
            sizeRanges.push_back("Miniscule: 0-1mb");
            sizeRanges.push_back("Tiny: 1-5mb");
            sizeRanges.push_back("Small: 5-25mb");
            sizeRanges.push_back("Medium: 25-100mb");
            sizeRanges.push_back("Large: 100-1.000mb");
            sizeRanges.push_back("Huge: 1-10gb");
            sizeRanges.push_back("Gigantic: 10-100gb");

            wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
            panelFiles.scrolledWindowFiles->SetSizer(boxSizer, true);

            std::vector<wxGenericCollapsiblePane*> colPanes;

            for (const auto& range : sizeRanges)
            {
                wxGenericCollapsiblePane* colPane = new wxGenericCollapsiblePane(panelFiles.scrolledWindowFiles, wxID_ANY, range, wxDefaultPosition, wxDefaultSize, wxCP_NO_TLW_RESIZE);
                colPane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this, colPane](wxCollapsiblePaneEvent& event)
                    {
                        panelFiles.Freeze();
                        colPane->GetPane()->Fit();
                        this->panelFiles.Layout();
                        this->panelFiles.Refresh();
                        panelFiles.Thaw();
                        event.Skip();
                    });
                wxGridSizer* gridSizer = new wxGridSizer(1, 0, 0);
                colPane->GetPane()->SetSizer(gridSizer);
                colPanes.push_back(colPane);
            }

            for (const auto& btn : arrContentButtons)
            {
                wxBoxSizer* sizerTile = new wxBoxSizer(wxHORIZONTAL);
                wxPanel* panel = new wxPanel(panelFiles.scrolledWindowFiles, wxID_ANY, wxDefaultPosition, wxSize(48, 44), wxBORDER_SIMPLE);
                panel->SetBackgroundColour(wxColor(232, 232, 232));
                panel->SetSizer(sizerTile);

                panel->Bind(wxEVT_LEFT_DOWN, [panel](wxMouseEvent& event)
                    {
                        printf("Click\n");
                        event.Skip();
                    });

                panel->Bind(wxEVT_ENTER_WINDOW, [panel](wxMouseEvent& event)
                    {
                        panel->SetBackgroundColour(wxColor(255, 255, 255));
                        panel->Refresh();
                        event.Skip();
                    });

                panel->Bind(wxEVT_LEAVE_WINDOW, [panel](wxMouseEvent& event)
                    {
                        panel->SetBackgroundColour(wxColor(232, 232, 232));
                        panel->Refresh();
                        event.Skip();
                    });

                btn->Reparent(panel);
                sizerTile->Add(btn, 0);

                std::filesystem::path* path = static_cast<std::filesystem::path*>(btn->GetClientData());

                auto fileSize = std::filesystem::file_size(*path);
                double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                wxString label(path->filename().wstring().c_str(), wxConvUTF8);
                wxString labelSize(wxString::Format("%.2f mb", fileSizeMb));

                wxStaticText* textName = new wxStaticText(panel, wxID_ANY, label, wxDefaultPosition, wxDefaultSize);
                wxStaticText* textSize = new wxStaticText(panel, wxID_ANY, labelSize, wxDefaultPosition, wxDefaultSize);

                wxBoxSizer* boxSizerText = new wxBoxSizer(wxVERTICAL);

                boxSizerText->Add(textName, 0);
                boxSizerText->Add(textSize, 0);

                sizerTile->Add(boxSizerText, 0, wxALIGN_CENTER);

                if (fileSizeMb < 1.0) // Miniscule
                {
                    panel->Reparent(colPanes[0]->GetPane());
                    colPanes[0]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (fileSizeMb < 5.0) // Tiny
                {
                    panel->Reparent(colPanes[1]->GetPane());
                    colPanes[1]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (fileSizeMb < 25.0) // Small
                {
                    panel->Reparent(colPanes[2]->GetPane());
                    colPanes[2]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (fileSizeMb < 100.0) // Medium
                {
                    panel->Reparent(colPanes[3]->GetPane());
                    colPanes[3]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (fileSizeMb < 1000.0) // Large
                {
                    panel->Reparent(colPanes[4]->GetPane());
                    colPanes[4]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else if (fileSizeMb < 10000.0) // Huge
                {
                    panel->Reparent(colPanes[5]->GetPane());
                    colPanes[5]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
                else // Gigantic
                {
                    panel->Reparent(colPanes[6]->GetPane());
                    colPanes[6]->GetPane()->GetSizer()->Add(panel, 0, wxEXPAND);
                }
            }

            for (const auto& pane : colPanes)
            {
                if (pane->GetPane()->GetSizer()->GetChildren().IsEmpty())
                {
                    boxSizer->Detach(pane);
                    pane->Destroy();
                    continue;
                }

                boxSizer->Add(pane, 0, wxEXPAND);

                pane->Expand();
            }
        }
        else {}
    }
    else if (viewMode == VIEW_DETAIL) {
        panelFilesSettings.choiceRecursive->Enable();
        panelFilesSettings.choiceSort->Enable();
        panelFilesSettings.choiceGroup->Enable();
        panelFilesSettings.choiceView->Enable();

        for (const auto& btn : arrBitmapButtons)
        {
            btn->Hide();
        }

        for (const auto& btn : arrContentButtons)
        {
            btn->Hide();
        }

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        panelFiles.scrolledWindowFiles->SetSizer(sizer, true);

        wxDataViewCtrl* dataCtrl = new wxDataViewCtrl(panelFiles.scrolledWindowFiles, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE | wxDV_ROW_LINES | wxDV_VERT_RULES);

        DataViewModelDetails* model = new DataViewModelDetails();

        dataCtrl->AppendTextColumn("Name", 0, wxDATAVIEW_CELL_INERT, 120);
        dataCtrl->AppendTextColumn("Size", 1, wxDATAVIEW_CELL_INERT, 50);

        if (groupMode == GROUP_NONE)
        {
            for (int i = 0; i < filePaths.size(); i++)
            {
                auto fileSize = std::filesystem::file_size(filePaths[i]);
                double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                wxString label(filePaths[i].filename().wstring().c_str(), wxConvUTF8);

                wxString labelSize(wxString::Format("%.2f mb", fileSizeMb));

                model->AddItem(label, labelSize);
            }
        }
        else if (groupMode == GROUP_TYPE)
        {
            std::vector<std::string> extTypes;
            bool exists;
            for (const auto& filePath : filePaths)
            {
                exists = false;
                for (const auto& ext : extTypes)
                {
                    if (ext == filePath.extension().string()) exists = true;
                    if (exists) break;
                }
                if (!exists)
                {
                    extTypes.push_back(filePath.extension().string());
                }
            }

            std::sort(extTypes.begin(), extTypes.end());

            std::vector<std::unique_ptr<DataViewModelDetailsNode>> folders;

            // Create a folder for each extension
            for (const auto& ext : extTypes)
            {
                auto folder = std::make_unique<DataViewModelDetailsNode>(ext, "", model->GetRoot());
                folders.push_back(std::move(folder));
            }
            // Add button to the right pane
            for (const auto& filePath : filePaths)
            {
                for (const auto& folder : folders)
                {
                    if (filePath.extension().string() == folder.get()->fileName)
                    {
                        auto fileSize = std::filesystem::file_size(filePath);
                        double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                        wxString label(filePath.filename().wstring().c_str(), wxConvUTF8);

                        wxString labelSize(wxString::Format("%.2f mb", fileSizeMb));

                        auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folder.get());

                        folder->children.push_back(std::move(entry));
                    }
                }
            }

            for (auto& folder : folders)
            {
                model->AddFolder(std::move(folder));
            }
        }
        else if (groupMode == GROUP_DATE)
        {
            std::vector<std::string> dateRanges;
            dateRanges.push_back("Today");
            dateRanges.push_back("Yesterday");
            dateRanges.push_back("Last week");
            dateRanges.push_back("Last month");
            dateRanges.push_back("Last 6 months");
            dateRanges.push_back("Last year");
            dateRanges.push_back("Before");

            std::vector<std::unique_ptr<DataViewModelDetailsNode>> folders;

            for (const auto& range : dateRanges)
            {
                auto folder = std::make_unique<DataViewModelDetailsNode>(range, "", model->GetRoot());
                folders.push_back(std::move(folder));
            }

            for (const auto& filePath : filePaths)
            {
                auto writeTime = std::filesystem::last_write_time(filePath);

                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    writeTime - decltype(writeTime)::clock::now() + std::chrono::system_clock::now()
                );

                auto now = std::chrono::system_clock::now();
                auto age = std::chrono::duration_cast<std::chrono::hours>(now - sctp).count();

                auto fileSize = std::filesystem::file_size(filePath);
                double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                wxString label(filePath.filename().wstring().c_str(), wxConvUTF8);

                wxString labelSize(wxString::Format("%.2f mb", fileSizeMb));

                if (age < 24) // Today
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[0].get());
                    folders[0]->children.push_back(std::move(entry));
                }
                else if (age < 48) // Yesterday
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[1].get());
                    folders[1]->children.push_back(std::move(entry));
                }
                else if (age < 24 * 7) // Last week
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[2].get());
                    folders[2]->children.push_back(std::move(entry));
                }
                else if (age < 24 * 30) // Last month
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[3].get());
                    folders[3]->children.push_back(std::move(entry));
                }
                else if (age < 24 * 180) // Last 6 months
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[4].get());
                    folders[4]->children.push_back(std::move(entry));
                }
                else if (age < 24 * 360) // Last year
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[5].get());
                    folders[5]->children.push_back(std::move(entry));
                }
                else // Before
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[6].get());
                    folders[6]->children.push_back(std::move(entry));
                }
            }

            for (auto& folder : folders)
            {
                if (folder->children.empty())
                {
                    folder.release();
                    continue;
                }

                model->AddFolder(std::move(folder));
            }
        }
        else if (groupMode == GROUP_SIZE)
        {
            std::vector<std::string> sizeRanges;
            sizeRanges.push_back("Miniscule: 0-1mb");
            sizeRanges.push_back("Tiny: 1-5mb");
            sizeRanges.push_back("Small: 5-25mb");
            sizeRanges.push_back("Medium: 25-100mb");
            sizeRanges.push_back("Large: 100-1.000mb");
            sizeRanges.push_back("Huge: 1-10gb");
            sizeRanges.push_back("Gigantic: 10-100gb");

            std::vector<std::unique_ptr<DataViewModelDetailsNode>> folders;

            for (const auto& range : sizeRanges)
            {
                auto folder = std::make_unique<DataViewModelDetailsNode>(range, "", model->GetRoot());
                folders.push_back(std::move(folder));
            }

            for (const auto& filePath : filePaths)
            {
                auto fileSize = std::filesystem::file_size(filePath);
                double fileSizeMb = static_cast<double>(fileSize) / (1024 * 1024);

                wxString label(filePath.filename().wstring().c_str(), wxConvUTF8);

                wxString labelSize(wxString::Format("%.2f mb", fileSizeMb));

                if (fileSizeMb < 1.0) // Miniscule
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[0].get());
                    folders[0]->children.push_back(std::move(entry));
                }
                else if (fileSizeMb < 5.0) // Tiny
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[1].get());
                    folders[1]->children.push_back(std::move(entry));
                }
                else if (fileSizeMb < 25.0) // Small
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[2].get());
                    folders[2]->children.push_back(std::move(entry));
                }
                else if (fileSizeMb < 100.0) // Medium
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[3].get());
                    folders[3]->children.push_back(std::move(entry));
                }
                else if (fileSizeMb < 1000.0) // Large
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[4].get());
                    folders[4]->children.push_back(std::move(entry));
                }
                else if (fileSizeMb < 10000.0) // Huge
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[5].get());
                    folders[5]->children.push_back(std::move(entry));
                }
                else // Gigantic
                {
                    auto entry = std::make_unique<DataViewModelDetailsNode>(label, labelSize, folders[6].get());
                    folders[6]->children.push_back(std::move(entry));
                }
            }

            for (auto& folder : folders)
            {
                if (folder->children.empty())
                {
                    folder.release();
                    continue;
                }

                model->AddFolder(std::move(folder));
            }
        }
        else {}

        dataCtrl->AssociateModel(model);
        model->ExpandAll(dataCtrl);

        model->DecRef();

        sizer->Add(dataCtrl, 1, wxEXPAND);
    }
    else if (viewMode == VIEW_TREE) {
        panelFilesSettings.choiceRecursive->Disable();
        panelFilesSettings.choiceSort->Disable();
        panelFilesSettings.choiceGroup->Disable();
        panelFilesSettings.choiceView->Enable();

        for (const auto& btn : arrBitmapButtons)
        {
            btn->Hide();
        }

        for (const auto& btn : arrContentButtons)
        {
            btn->Hide();
        }

        wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
        panelFiles.scrolledWindowFiles->SetSizer(boxSizer, true);

        wxGenericFileCtrl* fileCtrl = new wxGenericFileCtrl(panelFiles.scrolledWindowFiles, wxID_ANY, directory.string(), "", "*.*", wxFC_OPEN | wxFC_MULTIPLE, wxDefaultPosition, wxSize(100, 200));
        boxSizer->Add(fileCtrl, 1, wxEXPAND);

        if (groupMode == GROUP_NONE)
        {
        }
        else if (groupMode == GROUP_TYPE)
        {
        }
        else if (groupMode == GROUP_DATE)
        {
        }
        else if (groupMode == GROUP_SIZE)
        {
        }
        else {}
    }
    else {}
}

void FileManagerExtended::SortBitmapButtons()
{
    if (sortMode == NAME)
    {
        std::sort(arrBitmapButtons.begin(), arrBitmapButtons.end(), [](wxButton* a, wxButton* b) {
            auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
            auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
            return *pathA < *pathB;
            });
    }
    else if (sortMode == DATE)
    {
        std::sort(arrBitmapButtons.begin(), arrBitmapButtons.end(), [](wxButton* a, wxButton* b)
            {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                auto ftime_a = std::filesystem::last_write_time(*pathA);
                auto ftime_b = std::filesystem::last_write_time(*pathB);
                return ftime_a < ftime_b;
            });
    }
    else if (sortMode == SIZE)
    {
        std::sort(arrBitmapButtons.begin(), arrBitmapButtons.end(), [](wxButton* a, wxButton* b) {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                return std::filesystem::file_size(*pathA) < std::filesystem::file_size(*pathB);
            });
    }
    else if (sortMode == TYPE)
    {
        std::sort(arrBitmapButtons.begin(), arrBitmapButtons.end(), [](wxButton* a, wxButton* b)
            {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                return std::filesystem::path(*pathA).extension() < std::filesystem::path(*pathB).extension();
            });
    }
    else {}
}

void FileManagerExtended::SortContentButtons()
{
    if (sortMode == NAME)
    {
        std::sort(arrContentButtons.begin(), arrContentButtons.end(), [](wxButton* a, wxButton* b) {
            auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
            auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
            return *pathA < *pathB;
            });
    }
    else if (sortMode == DATE)
    {
        std::sort(arrContentButtons.begin(), arrContentButtons.end(), [](wxButton* a, wxButton* b)
            {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                auto ftime_a = std::filesystem::last_write_time(*pathA);
                auto ftime_b = std::filesystem::last_write_time(*pathB);
                return ftime_a < ftime_b;
            });
    }
    else if (sortMode == SIZE)
    {
        std::sort(arrContentButtons.begin(), arrContentButtons.end(), [](wxButton* a, wxButton* b) {
            auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
            auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
            return std::filesystem::file_size(*pathA) < std::filesystem::file_size(*pathB);
            });
    }
    else if (sortMode == TYPE)
    {
        std::sort(arrContentButtons.begin(), arrContentButtons.end(), [](wxButton* a, wxButton* b)
            {
                auto* pathA = static_cast<std::filesystem::path*>(a->GetClientData());
                auto* pathB = static_cast<std::filesystem::path*>(b->GetClientData());
                return std::filesystem::path(*pathA).extension() < std::filesystem::path(*pathB).extension();
            });
    }
    else {}
}

void FileManagerExtended::SortFilePaths()
{
    if (filePaths.empty()) return;

    if (sortMode == NAME)
    {
        std::sort(filePaths.begin(), filePaths.end());
    }
    else if (sortMode == DATE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                auto ftime_a = std::filesystem::last_write_time(a);
                auto ftime_b = std::filesystem::last_write_time(b);
                return ftime_a < ftime_b;
            });
    }
    else if (sortMode == SIZE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                return std::filesystem::file_size(a) < std::filesystem::file_size(b);
            });
    }
    else if (sortMode == TYPE)
    {
        std::sort(filePaths.begin(), filePaths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b)
            {
                return std::filesystem::path(a).extension() < std::filesystem::path(b).extension();
            });
    }
    else {}
}

void FileManagerExtended::CreateBitmapButtons()
{
    panelMediaCtrls.panelVcr->InitLoadingScreen();

    for (int i = 0; i < filePaths.size(); i++)
    {
        wxImage thumb;

        thumb = ImgUtils::GetThumbnail_ThumbnailCache(filePaths[i], 80, 51, ImgUtils::CENTER);

        if (!thumb.IsOk()) continue;

        ImgUtils::Borders_Rounded(thumb, 1);

        wxButton* btn = new wxButton(panelFiles.scrolledWindowFiles, wxID_ANY, "", wxDefaultPosition, wxSize(84, 55));
        btn->SetBitmap(wxBitmap(thumb));
        btn->SetClientData(new std::filesystem::path(filePaths[i]));

        btn->Bind(wxEVT_RIGHT_DOWN, [this, btn](wxMouseEvent& event)
            {
                wxMenu* menu = new wxMenu();
                menu->Append(gcrADD_TO_PLAYLIST, "Add to playlist");

                panelFiles.Bind(wxEVT_MENU, [this, btn](wxCommandEvent& event)
                    {
                        wxListItem listItem;
                        listItem.SetId(this->panelPlaylist.listCtrlPlaylist->GetItemCount());

                        std::filesystem::path* path = static_cast<std::filesystem::path*>(btn->GetClientData());
                        wxString label(path->filename().wstring().c_str(), wxConvUTF8);
                        
                        listItem.SetText(label);
                        listItem.SetData(path);
                        this->panelPlaylist.listCtrlPlaylist->InsertItem(listItem);
                    }, gcrADD_TO_PLAYLIST);

                panelFiles.PopupMenu(menu);
            });

        arrBitmapButtons.push_back(btn);

        std::string fileCount = "File: " + std::to_string(i + 1) + "/" + std::to_string(filePaths.size());

        panelMediaCtrls.panelVcr->RefreshLoadingScreen(fileCount);
    }
}

void FileManagerExtended::CreateContentButtons()
{
    panelMediaCtrls.panelVcr->InitLoadingScreen();

    for (int i = 0; i < filePaths.size(); i++)
    {
        wxImage thumb;

        thumb = ImgUtils::GetThumbnail_ThumbnailCache(filePaths[i], 52, 40, ImgUtils::CENTER);

        if (!thumb.IsOk()) continue;

        ImgUtils::Borders_Rounded(thumb, 1);

        wxButton* btn = new wxButton(panelFiles.scrolledWindowFiles, wxID_ANY, "", wxDefaultPosition, wxSize(56, 44));
        btn->SetBitmap(wxBitmap(thumb));
        btn->SetClientData(new std::filesystem::path(filePaths[i]));

        arrContentButtons.push_back(btn);

        std::string fileCount = "File: " + std::to_string(i + 1) + "/" + std::to_string(filePaths.size());

        panelMediaCtrls.panelVcr->RefreshLoadingScreen(fileCount);
    }
}

//-------------------//
//  wxDataViewModel  // 
//-------------------//

DataViewModelDetails::DataViewModelDetails()
{
    m_root = std::make_unique<DataViewModelDetailsNode>("Root", "0");
}

DataViewModelDetails::~DataViewModelDetails() = default;

void DataViewModelDetails::AddItem(wxString fileName, wxString size)
{
    auto entry = std::make_unique<DataViewModelDetailsNode>(fileName, size, m_root.get());
    m_root->children.push_back(std::move(entry));
}

void DataViewModelDetails::AddFolder(std::unique_ptr<DataViewModelDetailsNode> folder)
{
    m_root->children.push_back(std::move(folder));
}

void DataViewModelDetails::ExpandAll(wxDataViewCtrl* dataCtrl, const wxDataViewItem& parent)
{
    wxDataViewItemArray children;
    dataCtrl->GetModel()->GetChildren(parent, children);

    for (const auto& child : children)
    {
        dataCtrl->Expand(child);
        ExpandAll(dataCtrl, child);
    }
}

// Virtual
void DataViewModelDetails::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
{
    auto node = static_cast<DataViewModelDetailsNode*>(item.GetID());
    switch (col)
    {
    case 0: variant = node->fileName; break;
    case 1: variant = node->size; break;
    }
}

bool DataViewModelDetails::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
    auto node = static_cast<DataViewModelDetailsNode*>(item.GetID());
    switch (col)
    {
    case 0: node->fileName = variant.GetString(); return true;
    case 1: node->size = variant.GetString(); return true;
    }
    return false;
}

bool DataViewModelDetails::IsEnabled(const wxDataViewItem& item, unsigned int col) const
{
    return true;
}

wxDataViewItem DataViewModelDetails::GetParent(const wxDataViewItem& item) const
{
    if (!item.IsOk()) return wxDataViewItem();
    auto node = static_cast<DataViewModelDetailsNode*>(item.GetID());
    return wxDataViewItem(node->parent);
}

bool DataViewModelDetails::IsContainer(const wxDataViewItem& item) const
{
    if (!item.IsOk()) return true; // root is container
    auto node = static_cast<DataViewModelDetailsNode*>(item.GetID());
    return node->IsContainer();
}

unsigned int DataViewModelDetails::GetChildren(const wxDataViewItem& parent, wxDataViewItemArray& array) const
{
    DataViewModelDetailsNode* node = nullptr;

    if (!parent.IsOk())
        node = m_root.get();
    else
        node = static_cast<DataViewModelDetailsNode*>(parent.GetID());

    for (const auto& child : node->children)
        array.Add(wxDataViewItem(child.get()));

    return node->children.size();
}
