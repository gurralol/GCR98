#include "panel_files_settings.h"

PanelFilesSettings::PanelFilesSettings(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size, wxBORDER_RAISED)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);

	boxSizerCtrls1 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls2 = new wxBoxSizer(wxHORIZONTAL);

	staticBoxSizerRecursive = new wxStaticBoxSizer(wxVERTICAL, this, "Recursive");
	staticBoxSizerView = new wxStaticBoxSizer(wxVERTICAL, this, "View");
	staticBoxSizerSort = new wxStaticBoxSizer(wxVERTICAL, this, "Sort");
	staticBoxSizerGroup = new wxStaticBoxSizer(wxVERTICAL, this, "Group");

	wxString stringArrRecursive[] = { "No", "Yes" };
	choiceRecursive = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, stringArrRecursive);
	choiceRecursive->SetSelection(0);
	
	wxString stringArrView[] = {"Grid", "Content", "Details", "Tree"};
	choiceView = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, stringArrView);
	choiceView->SetSelection(0);

	wxString stringArrSort[] = { "Name", "Date", "Size", "Type" };
	choiceSort = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, stringArrSort);
	choiceSort->SetSelection(0);

	wxString stringArrGroup[] = { "None", "Type", "Date", "Size" };
	choiceGroup = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, stringArrGroup);
	choiceGroup->SetSelection(0);

	boxSizerMaster->Add(boxSizerCtrls1, 1, wxEXPAND);
	boxSizerMaster->Add(boxSizerCtrls2, 1, wxEXPAND);

	boxSizerCtrls1->Add(staticBoxSizerRecursive, 1, wxEXPAND | wxRIGHT, 2);
	boxSizerCtrls1->Add(staticBoxSizerSort, 1, wxEXPAND | wxLEFT, 1);

	boxSizerCtrls2->Add(staticBoxSizerView, 1, wxEXPAND | wxRIGHT, 2);
	boxSizerCtrls2->Add(staticBoxSizerGroup, 1, wxEXPAND | wxLEFT, 1);

	staticBoxSizerRecursive->Add(choiceRecursive, 1, wxEXPAND);
	staticBoxSizerSort->Add(choiceSort, 1, wxEXPAND);
	staticBoxSizerView->Add(choiceView, 1, wxEXPAND);
	staticBoxSizerGroup->Add(choiceGroup, 1, wxEXPAND);
}

PanelFilesSettings::~PanelFilesSettings()
{
}
