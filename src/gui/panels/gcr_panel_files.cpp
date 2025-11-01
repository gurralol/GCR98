#include "gcr_panel_files.h"

gcr_panel_files::gcr_panel_files(wxWindow* parent, wxWindowID id, wxPoint pos, wxSize size) 
	: gcr_panel_base(parent, id, pos, size)
{
	m_main_sizer = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(m_main_sizer);
	m_toolbar_sizer = new wxGridSizer(2, 0, 0);
	m_main_sizer->Add(m_toolbar_sizer, 0, wxEXPAND | wxALL, 5);

	m_sorting_options = new gcr_choice_base(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1));
	wxArrayString* choices = new wxArrayString();
	choices->Add("Sort by Name");
	choices->Add("Sort by Date Modified");
	choices->Add("Sort by Size");
	choices->Add("Sort by Type");
	m_sorting_options->Append(*choices);
	m_sorting_options->SetSelection(0);
	m_toolbar_sizer->Add(m_sorting_options, 0, wxEXPAND);

	m_filter_file_types = new gcr_choice_base(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1));
	wxArrayString* filter_choices = new wxArrayString();
	filter_choices->Add("All Files (*.*)");
	filter_choices->Add("Documents (*.doc; *.pdf; *.txt)");
	filter_choices->Add("Images (*.jpg; *.png; *.gif)");
	filter_choices->Add("Videos (*.mp4; *.avi; *.mkv)");
	m_filter_file_types->Append(*filter_choices);
	m_filter_file_types->SetSelection(0);
	m_toolbar_sizer->Add(m_filter_file_types, 0, wxEXPAND);
	
	m_view_modes = new gcr_choice_base(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1));
	wxArrayString* view_choices = new wxArrayString();
	view_choices->Add("Large Icons");
	view_choices->Add("Small Icons");
	view_choices->Add("List View");
	view_choices->Add("Details View");
	m_view_modes->Append(*view_choices);
	m_view_modes->SetSelection(0);
	m_toolbar_sizer->Add(m_view_modes, 0, wxEXPAND);
	
	m_grouping_options = new gcr_choice_base(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1));
	wxArrayString* grouping_choices = new wxArrayString();
	grouping_choices->Add("No Grouping");
	grouping_choices->Add("Group by Date");
	grouping_choices->Add("Group by Type");
	grouping_choices->Add("Group by Size");
	m_grouping_options->Append(*grouping_choices);
	m_grouping_options->SetSelection(0);
	m_toolbar_sizer->Add(m_grouping_options, 0, wxEXPAND);

	m_scrolled_window = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);
	m_scrolled_window->SetScrollRate(5, 5);
	m_main_sizer->Add(m_scrolled_window, 1, wxEXPAND | wxRIGHT | wxLEFT, 3);
}

gcr_panel_files::~gcr_panel_files()
{
}
