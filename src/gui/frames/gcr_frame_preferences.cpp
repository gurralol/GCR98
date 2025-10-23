#include "gcr_frame_preferences.h"

gcr_frame_preferences::gcr_frame_preferences(const wxString& title) : gcr_frame_base(title)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);

	m_aui_manager = new gcr_aui_manager(this, wxAUI_MGR_DEFAULT);
	m_panel_preferences = new gcr_panel_preferences(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_aui_manager->AddPane(m_panel_preferences, wxAuiPaneInfo().Name("Panel 1").Caption("").Center().CloseButton(false).MaximizeButton(false).MinSize(200, 200).BestSize(300, 300).PaneBorder(false));
	m_aui_manager->Update();

	Layout();
}

gcr_frame_preferences::~gcr_frame_preferences()
{
}
