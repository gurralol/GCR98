#include "theme_manager.h"

#include "../gui/gcr_panel_base.h"

std::vector<wxFrame*> gcr_theme_manager::m_registered_frames;

void gcr_theme_manager::register_member(wxFrame* frame)
{
	m_registered_frames.push_back(frame);
	printf("Registered frame for theming. Total registered: %zu\n", m_registered_frames.size());
}

void gcr_theme_manager::unregister_member(wxFrame* frame)
{
	m_registered_frames.erase(std::remove(m_registered_frames.begin(), m_registered_frames.end(), frame), m_registered_frames.end());
	printf("Unregistered frame from theming. Total registered: %zu\n", m_registered_frames.size());
}

void gcr_theme_manager::apply_theme_to_registered_members()
{
	for (wxFrame* frame : m_registered_frames)
	{
		apply_theme_recursively(frame);
	}
}

void gcr_theme_manager::apply_theme_recursively(wxWindow* window)
{
	if (dynamic_cast<wxPanel*>(window))
	{
		gcr_panel_base* panel = dynamic_cast<gcr_panel_base*>(window);
		panel->apply_theme(4);
		printf("Applied theme to panel: %s\n", panel->GetName().ToUTF8().data());
	}

	for (wxWindow* child : window->GetChildren())
	{
		apply_theme_recursively(child);
	}
}
