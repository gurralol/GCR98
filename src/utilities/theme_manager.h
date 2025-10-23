#pragma once
#include <vector>
#include <wx/wxprec.h>

class gcr_theme_manager
{
public:
	static std::vector<wxFrame*> m_registered_frames;

	static void register_member(wxFrame* frame);
	static void unregister_member(wxFrame* frame);

	static void apply_theme_to_registered_members();
	static void apply_theme_recursively(wxWindow* window);
};