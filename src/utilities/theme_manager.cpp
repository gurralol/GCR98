#include "theme_manager.h"

#include "../gui/gcr_panel_base.h"
#include "../gui/gcr_button_base.h"
#include "gcr_aui_manager.h"

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

void gcr_theme_manager::apply_theme_to_registered_members(int windowType, int selection)
{
	for (wxFrame* frame : m_registered_frames)
	{
		switch(windowType)
		{
			case 1:
				apply_theme_panels_recursively(frame, selection);
				break;
			case 2:
				// Apply border theme
				break;
			case 3:
				apply_theme_buttons_recursively(frame, selection);
				break;
			case 4:
				apply_theme_dockart_recursively(frame, selection);
				break;
			default:
				break;
		}
	}
}

void gcr_theme_manager::apply_theme_panels_recursively(wxWindow* window, int selection)
{
	if (dynamic_cast<wxPanel*>(window))
	{
		gcr_panel_base* panel = dynamic_cast<gcr_panel_base*>(window);
		panel->apply_theme(selection);
	}

	for (wxWindow* child : window->GetChildren())
	{
		apply_theme_panels_recursively(child, selection);
	}
}

void gcr_theme_manager::apply_theme_buttons_recursively(wxWindow* window, int selection)
{
	if (dynamic_cast<gcr_button_base*>(window))
	{
		gcr_button_base* panel = dynamic_cast<gcr_button_base*>(window);
		panel->apply_theme(selection);
	}

	for (wxWindow* child : window->GetChildren())
	{
		apply_theme_buttons_recursively(child, selection);
	}
}

void gcr_theme_manager::apply_theme_dockart_recursively(wxWindow* window, int selection)
{
	if (dynamic_cast<gcr_aui_manager*>(window))
	{
		gcr_aui_manager* panel = dynamic_cast<gcr_aui_manager*>(window);
		panel->apply_theme(selection);
	}

	for (wxWindow* child : window->GetChildren())
	{
		apply_theme_dockart_recursively(child, selection);
	}
}
