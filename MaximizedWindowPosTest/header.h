
#include "stdafx.h"
#include "windows.h"

enum LOG_TYPE {

    MONITOR_CHANGE,     // Log changes to  monitor (MonitorFromWindow)
    WINDOW_POS_CHANGE,  // Log changes to window pos changes (GetWindowRect)
    DPI,                // Log changes to window dpi (GetDpiForWindow)

    POS_CHANGING,       // Log WM_WINDOWPOSCHANGING WINDOWPOS/ flags
    POS_CHANGED,        // Log WM_WINDOWPOSCHANGED WINDOWPOS/ flags
    GET_MINMAXINFO,     // Log WM_GETMINMAXINFO

    CREATE,             // window creation

    ERR,                // errors
    MOUSE_BUTTON,        // puts some spaces in when you click

    LT_FIRST = MONITOR_CHANGE,
    LT_LAST = MOUSE_BUTTON,
};

void SetLogBit(LOG_TYPE lg, bool bSet);
void Log(LOG_TYPE type, CHAR* format, ...);
void LogWINDOWPOS(LOG_TYPE logtype, WINDOWPOS* pwp);
void LogMINMAXINFO(LOG_TYPE logtype, MINMAXINFO* pmmi);

char* WMnames(UINT msg);
const int cWINDOWPOSFLAGS = 13;
bool IsWindowFlagPresent(UINT flags, int i, char* & pstrFlagName);

class CWindow
{
public:
    CWindow(HWND _hwnd) : hwnd(_hwnd) { }

    void CheckWindowState();
    void SetCurrentMessage(UINT msg);

    void HandleCreate();
    void HandlePosChanging(WINDOWPOS* pwp);
    void HandlePosChanged(WINDOWPOS* pwp);
    void HandleMinMaxInfo(MINMAXINFO* pmmi);
    void HandleDPIChanged(UINT dpi, RECT* prc);
    void HandleGetDpiScaledSize(UINT dpi, PSIZE pSize);
    void HandleNCPaint();
    void HandlePaint(HDC hdc);

private:
    const SIZE StartSize = { 600, 400 };
    const COLORREF rgbBackground = RGB(50, 198, 239);

    bool UpdateMonitor();
    bool UpdateWindowRects();
    bool UpdateWindowDpi();

    UINT message;
    HWND hwnd = NULL;
    UINT dpi = 0;
    RECT rcWindow = {};
    RECT rcClient = {};
    HMONITOR hmon = NULL;
    MONITORINFO MonitorInfo = {};

    int ScaleForDpi(int val) { return MulDiv(val, dpi, 96); }
};


__inline CHAR* GetTextForWindow(HWND hwnd)
{
    //hwnd = GetAncestor(hwnd, GA_ROOT);
    static CHAR buf[100];
    PSTR pstr = (PSTR)&buf;
    GetWindowTextA(hwnd, pstr, sizeof(buf));
    return pstr;
}

#define WM_GETDPISCALEDSIZE             0x02E4
__inline UINT HackGetDpiForWindow(HWND hwnd)
{
    typedef UINT(WINAPI *fnGetDpiForWindow)(HWND);
    static fnGetDpiForWindow pfn = nullptr;

    if (!pfn)
    {
        HMODULE hModUser32 = GetModuleHandle(_T("user32.dll"));
        pfn = (fnGetDpiForWindow)GetProcAddress(hModUser32, "GetDpiForWindow");
        if (!pfn)
        {
            Log(DPI, "Can't find GetDpiForWindow.\n");
            PostQuitMessage(0);
            return 0;
        }
    }

    return pfn(hwnd);
}

__inline UINT HackSetThreadToPMV2()
{
#define DPI_AWARENESS_CONTEXT DWORD
    typedef DPI_AWARENESS_CONTEXT(WINAPI *fnSTDAC)(DPI_AWARENESS_CONTEXT);
    static fnSTDAC pfn = nullptr;

    if (!pfn)
    {
        HMODULE hModUser32 = GetModuleHandle(_T("user32.dll"));
        pfn = (fnSTDAC)GetProcAddress(hModUser32, "SetThreadDpiAwarenessContext");
        if (!pfn)
        {
            Log(DPI, "Can't find SetThreadDpiAwarenessContext.\n");
            PostQuitMessage(0);
            return 0;
        }
    }

    return pfn((DPI_AWARENESS_CONTEXT)-4);
}
