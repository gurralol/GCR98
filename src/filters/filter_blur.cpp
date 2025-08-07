#include "filter_blur.h"

FilterBlur::FilterBlur(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) :
	PanelFilter(parent, id, pos, size)
{
	textTitle->SetLabel("Blur");
}

FilterBlur::~FilterBlur()
{
}
