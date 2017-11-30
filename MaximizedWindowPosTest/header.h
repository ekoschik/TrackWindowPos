
#include "stdafx.h"
#include "windows.h"

enum LOG_TYPE {
    POS_CHANGING,
    POS_CHANGED,
    GET_MINMAXINFO,
    DPI,
    ERR,
    VERBOSE,

    LT_FIRST = POS_CHANGING,
    LT_LAST = VERBOSE,
};

void SetLogBit(LOG_TYPE lg, bool bSet);
void Log(LOG_TYPE type, CHAR* format, ...);
void LogWINDOWPOS(LOG_TYPE logtype, WINDOWPOS* pwp);
void LogMINMAXINFO(LOG_TYPE logtype, MINMAXINFO* pmmi);
char* WMnames(UINT msg);

class CWindow
{
public:
    CWindow(HWND _hwnd) : hwnd(_hwnd) { }

    void HandleCreate();
    void HandlePosChanging(WINDOWPOS* pwp);
    void HandlePosChanged(WINDOWPOS* pwp);
    void HandleMinMaxInfo(MINMAXINFO* pmmi);
    void HandleDPIChanged(UINT dpi, RECT* prc);
    void HandleGetDpiScaledSize(UINT dpi, PSIZE pSize);
    void HandlePaint(HDC hdc);
    void HandleButton(bool bLeft, bool bDown);

private:
    const SIZE StartSize = { 600, 400 };
    const COLORREF rgbBackground = RGB(50, 198, 239);

    HWND hwnd = NULL;
    RECT rcWindow = {};
    RECT rcClient = {};
    UINT dpi = 0;
};



//
// -- TODO: compile against latest sdk and remove all below
//

#define WM_GETDPISCALEDSIZE             0x02E4
__inline UINT GetDpiForWindow(HWND hwnd)
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



#define DPI_AWARENESS_CONTEXT_UNAWARE              ((DPI_AWARENESS_CONTEXT)-1)
#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE         ((DPI_AWARENESS_CONTEXT)-2)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT)-3)
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)

__inline UINT SetThreadDpiAwarenessContext(DWORD dpiContext)
{
#define DPI_AWARENESS_CONTEXT DWORD
    typedef DPI_AWARENESS_CONTEXT(WINAPI *fnSTDAC)(DPI_AWARENESS_CONTEXT);
    static fnSTDAC pfn = nullptr;

    if (!pfn)
    {
        HMODULE hModUser32 = GetModuleHandle(_T("user32.dll"));
        pfn = (fnSTDAC)GetProcAddress(hModUser32, "SetThreadDpiAwarenessContext");
    }

    if (!pfn)
    {
        Log(DPI, "ERROR! Can't find SetThreadDpiAwarenessContext!\n");
        return 0;
    }

    return pfn((DPI_AWARENESS_CONTEXT)dpiContext);
}
