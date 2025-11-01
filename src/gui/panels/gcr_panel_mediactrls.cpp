#include "gcr_panel_mediactrls.h"

gcr_panel_mediactrls::gcr_panel_mediactrls(wxWindow* parent, wxWindowID id, const wxPoint pos, const wxSize size) : gcr_panel_base(parent, id, pos, size)
{
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_mainSizer);

	m_sldPosition = new gcr_slider_base(this, wxID_ANY, 0, 0, 100, wxDefaultPosition, wxSize(400, 36));
	m_mainSizer->Add(m_sldPosition, 0, wxEXPAND | wxRIGHT | wxLEFT | wxTOP, 10);

	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSize btnSize = wxSize(84, 36);
	m_btnEject = new gcr_button_base(this, wxID_ANY, "EJECT", wxDefaultPosition, btnSize);
	m_btnRew = new gcr_button_base(this, wxID_ANY, "REWND", wxDefaultPosition, btnSize);
	m_btnPlay = new gcr_button_base(this, wxID_ANY, "PLAY", wxDefaultPosition, btnSize);
	m_btnStop = new gcr_button_base(this, wxID_ANY, "STOP", wxDefaultPosition, btnSize);
	m_btnForw = new gcr_button_base(this, wxID_ANY, "FORW", wxDefaultPosition, btnSize);
	m_btnRec = new gcr_button_base(this, wxID_ANY, "REC", wxDefaultPosition, btnSize);
	btnSizer->Add(m_btnEject, 0, wxALL, 5);
	btnSizer->Add(m_btnRew, 0, wxALL, 5);
	btnSizer->Add(m_btnPlay, 0, wxALL, 5);
	btnSizer->Add(m_btnStop, 0, wxALL, 5);
	btnSizer->Add(m_btnForw, 0, wxALL, 5);
	btnSizer->Add(m_btnRec, 0, wxALL, 5);

	m_mainSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 10);

	m_panelVCR = new gcr_panel_vcr(this, wxID_ANY, wxDefaultPosition, wxSize(400, 300));
	m_mainSizer->Add(m_panelVCR, 1, wxEXPAND | wxALL, 10);

	m_btnPlay->on_left_up_cb = [this](const wxMouseEvent& event) {
		if (on_play_cb) on_play_cb(event);
		};
}

gcr_panel_mediactrls::~gcr_panel_mediactrls()
{
}
