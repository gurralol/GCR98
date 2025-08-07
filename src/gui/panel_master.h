#pragma once
#include <wx/wxprec.h>

#include "../gui/panel_files.h"
#include "../gui/panel_files_settings.h"
#include "../gui/panel_filters.h"
#include "../gui/panel_filters_settings.h"
#include "../gui/panel_media.h"
#include "../gui/panel_mediactrls.h"
#include "../gui/panel_playlist.h"
#include "../gui/panel_render_settings.h"
#include "../gui/panel_vcr.h"

class PanelMaster : public wxPanel
{
public:
	PanelMaster(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelMaster();

    wxBoxSizer* boxSizerMaster;
    wxBoxSizer* boxSizerLeft;
    wxBoxSizer* boxSizerMiddle;
    wxBoxSizer* boxSizerRight;

    PanelFiles* panelFiles;
    PanelPlaylist* panelPlaylist;
    PanelMedia* panelMedia;
    PanelMediaCtrls* panelMediaCtrls;
    PanelFilters* panelFilters;
    PanelRenderSettings* panelRenderSettings;

    PanelFilesSettings* panelFilesSettings;
    PanelFiltersSettings* panelFiltersSettings;
};