#pragma once

#include <wx/wxprec.h>
#include "../gui/panel_filter.h"

class FilterBlur : public PanelFilter
{
public:
	FilterBlur(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size);
	~FilterBlur();
};