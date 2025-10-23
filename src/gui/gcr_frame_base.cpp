#include "gcr_frame_base.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <windows.h>

#include "../utilities/theme_manager.h"

bool IsWindowsInDarkMode()
{
    bool dark = false;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD value = 0, size = sizeof(value);
        if (RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, (LPBYTE)&value, &size) == ERROR_SUCCESS)
            dark = (value == 0);
        RegCloseKey(hKey);
    }
    return dark;
}

void SetDarkTitleBar(HWND hwnd, bool dark)
{
    if (!hwnd) return;

    BOOL useDark = dark ? TRUE : FALSE;

    // Windows 10 1809+ supports this attribute
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));
}

gcr_frame_base::gcr_frame_base(const wxString& title) : wxFrame(NULL, wxID_ANY, title)
{
	gcr_theme_manager::register_member(this);

#ifdef __WXMSW__
    SetDarkTitleBar((HWND)GetHWND(), IsWindowsInDarkMode());
#endif
}

gcr_frame_base::~gcr_frame_base()
{
	gcr_theme_manager::unregister_member(this);
}
