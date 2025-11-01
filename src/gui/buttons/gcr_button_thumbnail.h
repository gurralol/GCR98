#pragma once
#include <wx/wxprec.h>
#include "../gcr_button_base.h"

class gcr_button_thumbnail : public gcr_button_base
{
public:
	gcr_button_thumbnail(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size);
	~gcr_button_thumbnail();

	wxImage m_thumbnail_image;

private:
	virtual void on_paint(wxPaintEvent&) override;
};