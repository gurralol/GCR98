#pragma once
#include <wx/wxprec.h>
#include <chrono>

#include <vector>

class PanelVcr : public wxPanel
{
public:
	PanelVcr(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelVcr();

	wxStaticText* textLoading;

	void RenderDevStatistics();
	void InitLoadingScreen();
	void RefreshLoadingScreen(std::string text);
	void ClearVcr();

private:
	wxBoxSizer* boxSizerMaster;

	wxFont fontVcr = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "VT323");
	wxSize sizeTextVcr = wxSize(200, 18);
	wxColour colorTextVcr = wxColour(224, 224, 224);
};