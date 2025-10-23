#pragma once
#include <wx/wxprec.h>

class gcr_slider_base : public wxSlider
{
public:
	gcr_slider_base(wxWindow* parent, wxWindowID id, int value, int minValue, int maxValue, wxPoint pos, wxSize size);
	~gcr_slider_base();
};
