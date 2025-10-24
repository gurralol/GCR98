#include <wx/wxprec.h>
#include <wx/sysopt.h>
#include "gcr_app.h"
#include "gui/frames/gcr_frame_main.h"

wxIMPLEMENT_APP(gcr);

bool gcr::OnInit()
{
	// Show console for debugging output
    AllocConsole();
    void(freopen("CONOUT$", "w", stdout));
    void(freopen("CONOUT$", "w", stderr));

    if (!wxApp::OnInit()) return false;

	// Create the main application window
    gcr_frame_main* frame = new gcr_frame_main("Gu Cassette Recorder 98");

	// Set the frame size and position
    frame->SetInitialSize(wxWindow::FromDIP(wxSize(1100, 800), 0));
    frame->Center();

	// Set application icon
    wxIcon appIco("media/vcr16.ico", wxBITMAP_TYPE_ICO);
    frame->SetIcon(appIco);

    frame->Show();

    return true;
}
