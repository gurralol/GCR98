#include "panel_filters.h"

PanelFilters::PanelFilters(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size):
	wxPanel(parent, id, pos, size, wxBORDER_RAISED)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);
}

PanelFilters::~PanelFilters()
{

}
