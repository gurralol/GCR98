#include "panel_mediactrls.h"

PanelMediaCtrls::PanelMediaCtrls(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) : 
	wxPanel(parent, id, pos, size, wxBORDER_RAISED)
{
	wxFont::AddPrivateFont("media/fonts/VT323-Regular.ttf");

	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);

	boxSizerCtrls0 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls1 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls2 = new wxBoxSizer(wxHORIZONTAL);

	textPts = new wxStaticText(this, wxID_ANY, "00:00/00:00", wxDefaultPosition, wxSize(59, 16));
	sliderPts = new wxSlider(this, wxID_ANY, 0, 0, 1000, wxDefaultPosition, wxSize(30, 24), wxSL_SELRANGE);

	wxSize btnSize1 = wxSize(52, 32);
	wxSize btnSize2 = wxSize(52, 24);
	textVolume = new wxStaticText(this, wxID_ANY, "Vol", wxDefaultPosition, wxSize(16, 16));
	sliderVolume = new wxSlider(this, wxID_ANY, 10, 0, 20, wxDefaultPosition, wxSize(92, 24));
	btnPrevious = new wxButton(this, wxID_ANY, "PREV", wxDefaultPosition, btnSize1);
	btnBack = new wxButton(this, wxID_ANY, "BACK", wxDefaultPosition, btnSize1);
	btnStop = new wxButton(this, wxID_ANY, "STOP", wxDefaultPosition, btnSize1);
	btnPlayPause = new wxButton(this, wxID_ANY, "PLAY", wxDefaultPosition, btnSize1);
	btnForward = new wxButton(this, wxID_ANY, "FORW", wxDefaultPosition, btnSize1);
	btnNext = new wxButton(this, wxID_ANY, "NEXT", wxDefaultPosition, btnSize1);
	btnSeek = new wxToggleButton(this, wxID_ANY, "SEEK", wxDefaultPosition, btnSize1);
	btnLoop = new wxToggleButton(this, wxID_ANY, "LOOP", wxDefaultPosition, btnSize1);
	btnAutoPlay = new wxToggleButton(this, wxID_ANY, "AUTOP", wxDefaultPosition, btnSize1);

	btnMarkerJumpBack = new wxButton(this, wxID_ANY, "[<-", wxDefaultPosition, btnSize2);
	btnMarkerStart = new wxToggleButton(this, wxID_ANY, "[", wxDefaultPosition, btnSize2);
	btnRecord = new wxToggleButton(this, wxID_ANY, "REC", wxDefaultPosition, btnSize2);
	btnMarkerEnd = new wxToggleButton(this, wxID_ANY, "]", wxDefaultPosition, btnSize2);
	btnMarkerJumpForward = new wxButton(this, wxID_ANY, "->]", wxDefaultPosition, btnSize2);
	btnMarkerClear = new wxButton(this, wxID_ANY, "Clear", wxDefaultPosition, btnSize2);
	
	panelVcr = new PanelVcr(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	boxSizerMaster->Add(boxSizerCtrls0, 0, wxEXPAND);
	boxSizerMaster->Add(boxSizerCtrls1, 0, wxEXPAND);
	boxSizerMaster->Add(boxSizerCtrls2, 0, wxEXPAND);
	boxSizerMaster->Add(panelVcr, 1, wxEXPAND | wxALL, 9);

	boxSizerCtrls0->Add(textPts, 0, wxALIGN_CENTER);
	boxSizerCtrls0->Add(sliderPts, 1, wxALIGN_CENTER);

	boxSizerCtrls1->AddStretchSpacer();
	boxSizerCtrls1->Add(textVolume, 0, wxALIGN_CENTER);
	boxSizerCtrls1->Add(sliderVolume, 0, wxALIGN_CENTER);
	boxSizerCtrls1->Add(btnPrevious, 0);
	boxSizerCtrls1->Add(btnBack, 0);
	boxSizerCtrls1->Add(btnStop, 0);
	boxSizerCtrls1->Add(btnPlayPause, 0);
	boxSizerCtrls1->Add(btnForward, 0);
	boxSizerCtrls1->Add(btnNext, 0);
	boxSizerCtrls1->Add(btnSeek, 0);
	boxSizerCtrls1->Add(btnLoop, 0);
	boxSizerCtrls1->Add(btnAutoPlay, 0);
	boxSizerCtrls1->AddSpacer(56);
	boxSizerCtrls1->AddStretchSpacer();

	boxSizerCtrls2->AddStretchSpacer();
	boxSizerCtrls2->Add(btnMarkerJumpBack, 0);
	boxSizerCtrls2->Add(btnMarkerStart, 0);
	boxSizerCtrls2->Add(btnRecord, 0);
	boxSizerCtrls2->Add(btnMarkerEnd, 0);
	boxSizerCtrls2->Add(btnMarkerJumpForward, 0);
	boxSizerCtrls2->Add(btnMarkerClear, 0);
	boxSizerCtrls2->AddStretchSpacer();

	btnLoopVal = 0;
	btnLoop->Bind(wxEVT_TOGGLEBUTTON, &PanelMediaCtrls::LoopOnClick, this);
}

PanelMediaCtrls::~PanelMediaCtrls()
{

}

void PanelMediaCtrls::LoopOnClick(wxCommandEvent& event)
{
	btnLoopVal++;

	if (btnLoopVal == 1)
	{
		btnLoop->SetLabel("LOOP");
		btnLoop->SetValue(1);
	}
	else if (btnLoopVal == 2)
	{
		btnLoop->SetLabel("LOOP1");
		btnLoop->SetValue(1);
	}
	else if (btnLoopVal == 3)
	{
		btnLoop->SetLabel("SHFLE");
		btnLoop->SetValue(1);
	}
	else
	{
		btnLoop->SetLabel("LOOP");
		btnLoop->SetValue(0);
		btnLoopVal = 0;
	}

	event.Skip();
}
