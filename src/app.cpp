#include <wx/wxprec.h>
#include "app.h"
#include "mainframe.h"

wxIMPLEMENT_APP(Gcr98);

bool Gcr98::OnInit()
{
    /*AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);*/

    if (!wxApp::OnInit()) return false;

    MainFrame* frame = new MainFrame("Gu Cassette Recorder 98"); // Gu Cassette Recorder 98
    frame->SetInitialSize(wxSize(1100, 800));
    frame->Center();
    wxIcon appIco("media/vcr16.ico", wxBITMAP_TYPE_ICO);
    frame->SetIcon(appIco);
    frame->Show(true);

    return true;
}
