#include "gcr_aui_manager.h"

gcr_aui_manager::gcr_aui_manager(wxWindow* managed_wnd, unsigned int flags) : wxAuiManager(managed_wnd, flags)
{
	wxAuiDockArt* art = this->GetArtProvider();
	wxSize fontsize = wxWindow::FromDIP(wxSize(6,6), 0);
	art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);
	art->SetFont(wxAUI_DOCKART_CAPTION_FONT, wxFont(fontsize.x, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	art->SetColour(wxAUI_DOCKART_SASH_COLOUR, wxColour(32, 32, 32));
	art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, wxColour(32, 32, 32));
	art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxColour(16, 16, 16));
	art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(48, 48, 48));
	art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(24, 24, 24));
	art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, wxColour(200, 200, 200));
	art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, wxColour(128, 128, 128));
}

gcr_aui_manager::~gcr_aui_manager()
{
}

void gcr_aui_manager::apply_theme(int selection)
{
	switch (selection)
	{
	case 1:
		break;
	}
}