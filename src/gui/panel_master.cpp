#include "panel_master.h"

PanelMaster::PanelMaster(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
    wxPanel(parent, id, pos, size, wxBORDER_NONE)
{
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
}

PanelMaster::~PanelMaster()
{
}
