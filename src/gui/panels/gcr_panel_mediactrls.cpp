#include "gcr_panel_mediactrls.h"

gcr_panel_mediactrls::gcr_panel_mediactrls(wxWindow* parent, wxWindowID id, const wxPoint pos, const wxSize size) : gcr_panel_base(parent, id, pos, size)
{
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_mainSizer);

	m_sldPosition = new gcr_slider_base(this, wxID_ANY, 0, 0, 100, wxDefaultPosition, wxSize(400, 36));
	m_mainSizer->Add(m_sldPosition, 0, wxEXPAND | wxRIGHT | wxLEFT | wxTOP, 10);

	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
	wxSize btnSize = wxSize(84, 36);
	m_sldVolume = new gcr_slider_base(this, wxID_ANY, 100, 0, 100, wxDefaultPosition, wxSize(150, 36));
	m_btnEject = new gcr_button_base(this, wxID_ANY, "EJECT", wxDefaultPosition, btnSize);
	m_btnRew = new gcr_button_base(this, wxID_ANY, "REWND", wxDefaultPosition, btnSize);
	m_btnPlay = new gcr_button_base(this, wxID_ANY, "PLAY", wxDefaultPosition, btnSize);
	m_btnStop = new gcr_button_base(this, wxID_ANY, "STOP", wxDefaultPosition, btnSize);
	m_btnForw = new gcr_button_base(this, wxID_ANY, "FORW", wxDefaultPosition, btnSize);
	m_btnLoop = new gcr_button_base(this, wxID_ANY, "LOOP", wxDefaultPosition, btnSize);
	m_btnRec = new gcr_button_base(this, wxID_ANY, "REC", wxDefaultPosition, btnSize);
	btnSizer->Add(m_sldVolume, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	btnSizer->Add(m_btnEject, 0, wxALL, 5);
	btnSizer->Add(m_btnRew, 0, wxALL, 5);
	btnSizer->Add(m_btnPlay, 0, wxALL, 5);
	btnSizer->Add(m_btnStop, 0, wxALL, 5);
	btnSizer->Add(m_btnForw, 0, wxALL, 5);
	btnSizer->Add(m_btnLoop, 0, wxALL, 5);
	btnSizer->Add(m_btnRec, 0, wxALL, 5);

	m_mainSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 10);

	m_panelVCR = new gcr_panel_vcr(this, wxID_ANY, wxDefaultPosition, wxSize(400, 300));
	m_mainSizer->Add(m_panelVCR, 1, wxEXPAND | wxALL, 10);

	m_btnPlay->on_left_up_cb = [this](const wxMouseEvent& event) {
		if (on_play_cb) on_play_cb(event);
		};

	m_btnLoop->on_left_up_cb = [this](const wxMouseEvent& event) {
		static int loop_state = 0;
		loop_state = (loop_state + 1) % 4;
		switch (loop_state)
		{
		case 0:
			m_btnLoop->SetLabel("LOOP");
			m_btnLoop->m_toggle_state = 0;
			break;
		case 1:
			m_btnLoop->SetLabel("LOOP");
			m_btnLoop->m_toggle_state = 1;
			break;
		case 2:
			m_btnLoop->SetLabel("LOOP1");
			m_btnLoop->m_toggle_state = 2;
			break;
		case 3:
			m_btnLoop->SetLabel("SHFFL");
			m_btnLoop->m_toggle_state = 3;
			break;
		}
		};
}

gcr_panel_mediactrls::~gcr_panel_mediactrls()
{
}
