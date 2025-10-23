#pragma once
#include <wx/wxprec.h>
#include "../gcr_panel_base.h"
#include "../gcr_slider_base.h"
#include "../gcr_button_base.h"
#include "gcr_panel_vcr.h"

class gcr_panel_mediactrls : public gcr_panel_base
{
public:
	gcr_panel_mediactrls(wxWindow* parent, wxWindowID id, const wxPoint pos, const wxSize size);
	~gcr_panel_mediactrls();

private:
	wxBoxSizer* m_mainSizer;

	gcr_slider_base* m_sldPosition;

	gcr_button_base* m_btnEject;
	gcr_button_base* m_btnRew;
	gcr_button_base* m_btnPlay;
	gcr_button_base* m_btnStop;
	gcr_button_base* m_btnForw;
	gcr_button_base* m_btnRec;

	gcr_panel_vcr* m_panelVCR;
};