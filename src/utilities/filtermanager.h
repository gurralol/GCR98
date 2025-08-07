#pragma once
#include <wx/wxprec.h>
#include <wx/dir.h>
#include "../gui/panel_filter.h"

class FilterManager
{
public:
	FilterManager();
	~FilterManager();

	std::vector<PanelFilter*> arrFilters;
};