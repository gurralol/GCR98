#include <wx/collpane.h>
#include "panel_files.h"

PanelFiles::PanelFiles(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size, wxBORDER_RAISED)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);

	scrolledWindowFiles = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	scrolledWindowFiles->SetScrollRate(1, 10);

	boxSizerMaster->Add(scrolledWindowFiles, 1, wxEXPAND);
}

PanelFiles::~PanelFiles()
{

}
