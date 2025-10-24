#pragma once
#include <vector>
#include <wx/wxprec.h>

class gcr_theme_manager
{
public:
	static std::vector<wxFrame*> m_registered_frames;

	static void register_member(wxFrame* frame);
	static void unregister_member(wxFrame* frame);

	static void apply_theme_to_registered_members(int windowType, int selection);

	static void apply_theme_panels_recursively(wxWindow* window, int selection);
	static void apply_theme_buttons_recursively(wxWindow* window, int selection);
	static void apply_theme_dockart_recursively(wxWindow* window, int selection);
};