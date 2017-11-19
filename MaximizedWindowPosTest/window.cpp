
#include "stdafx.h"
#include "windows.h"
#include "header.h"
#include "ShellScalingAPI.h"


bool CWindow::UpdateMonitor()
{
    HMONITOR hmonPrev = hmon;
    hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    bool bChange = (hmonPrev != hmon);

    if (bChange)
    {
        MonitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hmon, &MonitorInfo);

        UINT dpi;
        GetDpiForMonitor(hmon, MDT_EFFECTIVE_DPI, &dpi, &dpi);

        Log(MONITOR_CHANGE, "New Monitor (hMonitor 0x%X)\n", hmon);
        Log(MONITOR_CHANGE, " - current window message %s\n", WMnames(message));
        Log(MONITOR_CHANGE, " - monitor DPI %i\n", dpi);

        PRECT prc = &MonitorInfo.rcMonitor;
        Log(MONITOR_CHANGE, " - rcMonitor: [%i, %i, %i, %i] (%i x %i)\n",
            prc->left, prc->top, prc->right, prc->bottom,
            prc->right - prc->left, prc->bottom - prc->top);

        prc = &MonitorInfo.rcWork;
        Log(MONITOR_CHANGE, " - rcWork: [%i, %i, %i, %i] (%i x %i)\n",
            prc->left, prc->top, prc->right, prc->bottom,
            prc->right - prc->left, prc->bottom - prc->top);

        if (MonitorInfo.dwFlags & MONITORINFOF_PRIMARY)
        {
            Log(MONITOR_CHANGE, " - PRIMARY\n");
        }
    }

    return bChange;
}

bool CWindow::UpdateWindowRects()
{
    RECT rcWndPrev = rcWindow;
    GetWindowRect(hwnd, &rcWindow);
    GetClientRect(hwnd, &rcClient);
    bool bChange = (!EqualRect(&rcWindow, &rcWndPrev));

    if (bChange)
    {
        Log(WINDOW_POS_CHANGE, "New Window Pos, current window message %s\n", WMnames(message));
        Log(WINDOW_POS_CHANGE, " - [%i, %i, %i, %i] (%i x %i)\n",
            rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom,
            rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top);
    }

    return bChange;
}

bool CWindow::UpdateWindowDpi()
{
    UINT newDpi = HackGetDpiForWindow(hwnd);
    bool bChange = (newDpi != dpi);
    dpi = newDpi;

    if (bChange)
    {
        Log(DPI, "New Window DPI %i\n", dpi);
        Log(DPI, " - current window message %s\n", WMnames(message));
    }

    return bChange;
}


void CWindow::HandleCreate()
{
    Log(CREATE, "Entering WM_CREATE handler.\n");

    UpdateMonitor();

    UpdateWindowDpi();

    UpdateWindowRects();

    int cx = ScaleForDpi(StartSize.cx);
    int cy = ScaleForDpi(StartSize.cy);
    SetWindowPos(hwnd, 0, 0, 0, cx, cy, SWP_SHOWWINDOW | SWP_NOMOVE);

    Log(CREATE, "Exiting WM_CREATE handler.\n");
    Log(CREATE, " - initial window monitor 0x%X\n");
    Log(CREATE, " - initial window dpi: %i\n", dpi);

    int cxfinal = rcWindow.right - rcWindow.left;
    int cyfinal = rcWindow.bottom - rcWindow.top;
    Log(CREATE, " - initial window size: %i x %i\n", cxfinal, cyfinal);

    if (cx != cxfinal || cy != cyfinal)
    {
        Log(ERR, "Error: starting size (%i x %i) was not the expected size (%i x %i).\n",
            cxfinal, cyfinal, cx, cy);
    }
}

void CWindow::HandlePosChanging(WINDOWPOS* pwp)
{
    Log(POS_CHANGING, "HANDLING WM_WINDOWPOSCHANGING.\n");
    LogWINDOWPOS(POS_CHANGING, pwp);
}

void CWindow::HandlePosChanged(WINDOWPOS* pwp)
{
    Log(POS_CHANGED, "HANDLING WM_WINDOWPOSCHANGED.\n");
    LogWINDOWPOS(POS_CHANGED, pwp);
}

void CWindow::HandleNCPaint()
{

}

void CWindow::HandleMinMaxInfo(MINMAXINFO* pmmi)
{
    Log(GET_MINMAXINFO, "HANDLING WM_GETMINMAXINFO.\n");
    LogMINMAXINFO(GET_MINMAXINFO, pmmi);
}

void CWindow::HandleDPIChanged(UINT dpiMSg, RECT* prc)
{
    Log(DPI, "HANDLING WM_DPICHANGED.\n");

    UpdateWindowDpi();
    if (dpi != dpiMSg)
    {
        Log(ERR, "HandleDPIChanged passed dpi != GetDpiForWindow\n");
    }

    Log(DPI, " - suggestion rect size: %i x %i\n",
        prc->right - prc->left, prc->bottom - prc->top);

    UINT dpiPreDpiChange = dpi;
    HMONITOR hMonPreDpiChange = hmon;

    SetWindowPos(hwnd, nullptr,
        prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top,
        SWP_NOZORDER | SWP_NOACTIVATE);

    if (dpiPreDpiChange != dpi)
    {
        Log(ERR, "Suggestion rect in DPI change caused a DPI change!\n");
    }

    if (hMonPreDpiChange != hmon)
    {
        Log(ERR, "Suggestion rect in DPI change caused a monitor change!\n");
    }

    Log(DPI, " - exiting DPI Change handler\n");
}

void CWindow::HandleGetDpiScaledSize(UINT dpi, PSIZE pSize)
{
    Log(DPI, "HANDLING WM_GETDPISCALEDSIZE.\n");
    Log(DPI, " - proposed DPI: %i\n", dpi);
    Log(DPI, " - proposed size: %i x %i\n", pSize->cx, pSize->cy);

    // TODO muck with proposed size

}

void CWindow::HandlePaint(HDC hdc)
{
    static HBRUSH hbr = CreateSolidBrush(rgbBackground);
    FillRect(hdc, &rcClient, hbr);

    // TODO maybe have some way of toggling logging types in the client area?

}

void CWindow::CheckWindowState()
{
    UpdateWindowRects();

    UpdateMonitor();

    UpdateWindowDpi();
}

void CWindow::SetCurrentMessage(UINT msg)
{
    message = msg;
}