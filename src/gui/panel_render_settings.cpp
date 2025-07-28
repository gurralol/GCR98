#include "panel_render_settings.h"

PanelRenderSettings::PanelRenderSettings(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	wxPanel(parent, id, pos, size, wxBORDER_RAISED)
{
	boxSizerMaster = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(boxSizerMaster);

	wxSize textSize = wxSize(50, 20);

	boxSizerCtrls0 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls05 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls1 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls2 = new wxBoxSizer(wxHORIZONTAL);
	boxSizerCtrls3 = new wxBoxSizer(wxHORIZONTAL);
	staticBoxSizerCtrls0 = new wxStaticBoxSizer(wxHORIZONTAL, this, "Output directory");

	textRenderSettings = new wxStaticText(this, wxID_ANY, "Render settings", wxDefaultPosition, textSize);
	staticLineRenderSettings = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(32, 1));

	boxSizerMaster->Add(textRenderSettings, 0, wxEXPAND);
	boxSizerMaster->Add(staticLineRenderSettings, 0, wxEXPAND | wxBOTTOM, 6);
	boxSizerMaster->Add(boxSizerCtrls0, 0, wxEXPAND);
	boxSizerMaster->Add(boxSizerCtrls05, 0, wxEXPAND);
	boxSizerMaster->Add(boxSizerCtrls1, 0, wxEXPAND);
	boxSizerMaster->Add(boxSizerCtrls2, 0, wxEXPAND);
	boxSizerMaster->Add(boxSizerCtrls3, 0, wxEXPAND);
	boxSizerMaster->Add(staticBoxSizerCtrls0, 0, wxEXPAND | wxTOP, 2);

	textFileType = new wxStaticText(this, wxID_ANY, "Filetype", wxDefaultPosition, textSize);
	textCompression = new wxStaticText(this, wxID_ANY, "Compression", wxDefaultPosition, textSize);
	textFps = new wxStaticText(this, wxID_ANY, "FPS", wxDefaultPosition, textSize);
	textResolution = new wxStaticText(this, wxID_ANY, "Resolution", wxDefaultPosition, textSize);
	textFit = new wxStaticText(this, wxID_ANY, "Fit", wxDefaultPosition, textSize);

	wxString stringArrayFileType[] = { "MP4", "WEBM", "AVI", "GIF", "WEBP" };
	choiceFileType = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(50, 24), 5, stringArrayFileType);
	choiceFileType->SetSelection(0);

	spinCtrlCompression = new wxSpinCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(25, 24));
	spinCtrlCompression->SetValue(90);

	spinCtrlFps = new wxSpinCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(25, 24));
	spinCtrlFps->SetValue(30);

	wxString stringArrayFps[] = { "Custom", "Source", "6", "12", "18", "24", "30", "48", "60" };
	choiceFps = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(25, 24), 9, stringArrayFps);
	choiceFps->SetSelection(1);

	wxString stringArrayResolution[] =
	{
		"Source",
		"240p 16:9",
		"360p 16:9",
		"480p 16:9",
		"720p 16:9",
		"1080p 16:9",
		"1440p 16:9",
		"2160p 16:9",
		"4320p 16:9"
	};
	choiceResolution = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(50, 24), 9, stringArrayResolution);
	choiceResolution->SetSelection(0);

	wxString stringArrayFit[] = {"Resize", "Padding", "Fill"};
	choiceFit = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(50, 24), 3, stringArrayFit);
	choiceFit->SetSelection(0);

	dirPickerOutput = new wxDirPickerCtrl(this, wxID_ANY, "", "Browse", wxDefaultPosition, wxDefaultSize);

	boxSizerCtrls0->Add(textFileType, 2, wxALIGN_CENTER);
	boxSizerCtrls0->Add(choiceFileType, 3, wxALIGN_CENTER);

	boxSizerCtrls05->Add(textCompression, 2, wxALIGN_CENTER);
	boxSizerCtrls05->Add(spinCtrlCompression, 3, wxALIGN_CENTER);

	boxSizerCtrls1->Add(textFps, 6, wxALIGN_CENTER);
	boxSizerCtrls1->Add(spinCtrlFps, 3, wxALIGN_CENTER);
	boxSizerCtrls1->Add(choiceFps, 6, wxALIGN_CENTER);

	boxSizerCtrls2->Add(textResolution, 2, wxALIGN_CENTER);
	boxSizerCtrls2->Add(choiceResolution, 3, wxEXPAND);

	boxSizerCtrls3->Add(textFit, 2, wxALIGN_CENTER);
	boxSizerCtrls3->Add(choiceFit, 3, wxALIGN_CENTER);

	staticBoxSizerCtrls0->Add(dirPickerOutput, 1, wxALIGN_CENTER);
}

PanelRenderSettings::~PanelRenderSettings()
{

}
