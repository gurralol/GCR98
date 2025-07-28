#include "panel_vcr.h"

PanelVcr::PanelVcr(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size, wxBORDER_DOUBLE)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);

	this->SetBackgroundColour(wxColor(32, 32, 32));
}

PanelVcr::~PanelVcr()
{
	
}

void PanelVcr::RenderDevStatistics()
{

}

void PanelVcr::InitLoadingScreen()
{
	textLoading = new wxStaticText(this, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
	textLoading->SetFont(fontVcr);
	textLoading->SetForegroundColour(colorTextVcr);

	this->boxSizerMaster->Clear(true);
	this->boxSizerMaster->AddStretchSpacer();
	this->boxSizerMaster->Add(textLoading, 0, wxALIGN_CENTER);
	this->boxSizerMaster->AddStretchSpacer();

	this->Layout();
	this->Refresh();
}

void PanelVcr::RefreshLoadingScreen(std::string text)
{
	textLoading->SetLabel(text);
	
	this->Layout();
	this->Refresh();
}

void PanelVcr::ClearVcr()
{
	this->boxSizerMaster->Clear(true);
}
