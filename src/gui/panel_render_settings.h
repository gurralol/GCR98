#pragma once
#include <wx/wxprec.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>

class PanelRenderSettings : public wxPanel
{
public:
	PanelRenderSettings(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~PanelRenderSettings();

	wxBoxSizer* boxSizerMaster;
	wxBoxSizer* boxSizerCtrls0;
	wxBoxSizer* boxSizerCtrls05;
	wxBoxSizer* boxSizerCtrls1;
	wxBoxSizer* boxSizerCtrls2;
	wxBoxSizer* boxSizerCtrls3;
	wxStaticBoxSizer* staticBoxSizerCtrls0;

	wxStaticText* textRenderSettings;
	wxStaticText* textFileType;
	wxStaticText* textCompression;
	wxStaticText* textFps;
	wxStaticText* textResolution;
	wxStaticText* textFit;

	wxStaticLine* staticLineRenderSettings;

	wxChoice* choiceFileType;

	wxSpinCtrl* spinCtrlCompression;

	wxSpinCtrl* spinCtrlFps;
	wxChoice* choiceFps;

	wxChoice* choiceResolution;

	wxChoice* choiceFit;

	wxDirPickerCtrl* dirPickerOutput;

	// Filetype
	// Fps
	// Resolution: Presets / Scale of original % / [x] [y]
	// Fit (resize, padding, fill)
	// Output dir
};