#include "gcr_slider_base.h"

gcr_slider_base::gcr_slider_base(wxWindow* parent, wxWindowID id, int value, int minValue, int maxValue, wxPoint pos, wxSize size)
	: wxSlider(parent, id, value, minValue, maxValue, pos, size)
{
}

gcr_slider_base::~gcr_slider_base()
{
}
