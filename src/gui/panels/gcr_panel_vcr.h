#pragma once
#include <wx/wxprec.h>
#include "../gcr_panel_base.h"

class gcr_panel_vcr : public gcr_panel_base
{
public:
	gcr_panel_vcr(wxWindow* parent, wxWindowID id, wxPoint pos, const wxSize size);
	~gcr_panel_vcr();

	wxBoxSizer* m_mainSizer;

private:
	void on_paint(wxPaintEvent& event) override;
};