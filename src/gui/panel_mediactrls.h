#pragma once
#include <wx/wxprec.h>
#include <wx/tglbtn.h>

#include "panel_vcr.h"

class PanelMediaCtrls : public wxPanel
{
public:
	PanelMediaCtrls(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelMediaCtrls();

	wxBoxSizer* boxSizerMaster;
	wxBoxSizer* boxSizerCtrls0;
	wxBoxSizer* boxSizerCtrls1;
	wxBoxSizer* boxSizerCtrls2;

	wxStaticText* textPts;
	wxSlider* sliderPts;

	wxStaticText* textVolume;
	wxSlider* sliderVolume;
	wxButton* btnPrevious;
	wxButton* btnBack;
	wxButton* btnStop;
	wxButton* btnPlayPause;
	wxButton* btnForward;
	wxButton* btnNext;
	wxToggleButton* btnLoop;
	wxToggleButton* btnAutoPlay;
	wxToggleButton* btnSeek;

	wxToggleButton* btnRecord;
	wxToggleButton* btnMarkerStart;
	wxToggleButton* btnMarkerEnd;
	wxButton* btnMarkerJumpBack;
	wxButton* btnMarkerJumpForward;
	wxButton* btnMarkerClear;

	PanelVcr* panelVcr;

	int btnLoopVal;

	void LoopOnClick(wxCommandEvent& event);
};