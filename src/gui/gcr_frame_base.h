#pragma once
#include <wx/wxprec.h>

class gcr_frame_base : public wxFrame
{
public:
	gcr_frame_base(const wxString& title);
	~gcr_frame_base();
};