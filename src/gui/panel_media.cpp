#include "panel_media.h"

PanelMedia::PanelMedia(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);

	this->SetBackgroundStyle(wxBG_STYLE_PAINT);
	this->SetBackgroundColour(wxColor(0, 0, 0));
}

PanelMedia::~PanelMedia()
{

}
