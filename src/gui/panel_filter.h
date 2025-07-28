#pragma once
#include <wx/wxprec.h>
#include <wx/spinbutt.h>
#include <wx/tglbtn.h>
#include <wx/statline.h>

class PanelFilter : public wxPanel
{
public:
	PanelFilter(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelFilter();

	wxBoxSizer* boxSizerContent;
	//wxString textName;

private:
	wxBoxSizer* boxSizerMaster;

	wxBoxSizer* boxSizerTopbar;
	wxBoxSizer* boxSizerPresets;
	//wxBoxSizer* boxSizerAuthor;

	wxPanel* panelTopbar;
	wxPanel* panelPresets;
	wxPanel* panelContent;

	wxStaticText* textTitle;
	wxSpinButton* spinBtnMove;
	wxButton* btnMinimize;
	wxToggleButton* btnBypass;
	wxButton* btnDestroy;

	wxStaticText* textPresets;
	wxButton* btnPresetSave;
	wxButton* btnPresetDelete;
	wxArrayString presetArr;
	wxChoice* choicePreset;

	wxStaticLine* staticLine1;

	//wxStaticText* textAuthor;
};