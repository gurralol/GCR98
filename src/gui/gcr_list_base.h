#pragma once
#include <wx/wxprec.h>
#include <wx/listctrl.h>

class gcr_list_base : public wxListCtrl
{
public:
	gcr_list_base(wxWindow* parent, wxWindowID id, const wxPoint pos, const wxSize size);
	~gcr_list_base();
};