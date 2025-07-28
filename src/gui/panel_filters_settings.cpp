#include "panel_filters_settings.h"

PanelFiltersSettings::PanelFiltersSettings(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size, wxBORDER_RAISED)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);

	boxSizerCtrls1 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls2 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls3 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls4 = new wxBoxSizer(wxHORIZONTAL);

	//staticBoxSizer0 = new wxStaticBoxSizer(wxVERTICAL, this, "Bypass type");

	wxSize btnSize = wxSize(96, 23);
	btnBypassAll = new wxButton(this, wxID_ANY, "Bypass all", wxDefaultPosition, btnSize);
	btnAllowAll = new wxButton(this, wxID_ANY, "Allow all", wxDefaultPosition, btnSize);
	btnRemoveAll = new wxButton(this, wxID_ANY, "Remove all", wxDefaultPosition, btnSize);
	btnLoadStack = new wxButton(this, wxID_ANY, "Load stack", wxDefaultPosition, btnSize);
	btnSaveStack = new wxButton(this, wxID_ANY, "Save stack", wxDefaultPosition, btnSize);
	btnCollapseAll = new wxButton(this, wxID_ANY, "Collapse all", wxDefaultPosition, btnSize);
	btnExpandAll = new wxButton(this, wxID_ANY, "Expand all", wxDefaultPosition, btnSize);
	
	wxString stringArrBypassByType[] = { "All", "Color", "AI", "Realtime", "Non-realtime"};
	choiceBypassByType = new wxChoice(this, wxID_ANY, wxDefaultPosition, btnSize, 5, stringArrBypassByType);
	choiceBypassByType->SetSelection(0);

	boxSizerMaster->AddStretchSpacer();
	boxSizerMaster->Add(boxSizerCtrls1, 0, wxALIGN_CENTER);
	boxSizerMaster->Add(boxSizerCtrls2, 0, wxALIGN_CENTER);
	boxSizerMaster->Add(boxSizerCtrls3, 0, wxALIGN_CENTER);
	boxSizerMaster->Add(boxSizerCtrls4, 0, wxALIGN_CENTER);
	boxSizerMaster->AddStretchSpacer();

	boxSizerCtrls1->Add(btnBypassAll, 0, wxALIGN_CENTER);
	boxSizerCtrls1->Add(choiceBypassByType, 0, wxALIGN_CENTER);

	boxSizerCtrls2->Add(btnAllowAll, 0, wxALIGN_CENTER);
	boxSizerCtrls2->Add(btnRemoveAll, 0, wxALIGN_CENTER);

	boxSizerCtrls3->Add(btnCollapseAll, 0, wxALIGN_CENTER);
	boxSizerCtrls3->Add(btnExpandAll, 0, wxALIGN_CENTER);

	boxSizerCtrls4->Add(btnLoadStack, 0, wxALIGN_CENTER);
	boxSizerCtrls4->Add(btnSaveStack, 0, wxALIGN_CENTER);
}

PanelFiltersSettings::~PanelFiltersSettings()
{
}
