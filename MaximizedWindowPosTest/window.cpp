
#include "stdafx.h"
#include "windows.h"
#include "header.h"
#include "ShellScalingAPI.h"

void CWindow::HandleCreate()
{
    Log(VERBOSE, "Handling WM_CREATE.\n");

    dpi = GetDpiForWindow(hwnd);
    int cx = MulDiv(StartSize.cx, dpi, 96);
    int cy = MulDiv(StartSize.cy, dpi, 96);
    Log(VERBOSE, "Setting initial size: %i x %i\n", cx, cy);
    SetWindowPos(hwnd, 0, 0, 0, cx, cy, SWP_SHOWWINDOW | SWP_NOMOVE);

    GetWindowRect(hwnd, &rcWindow);
    int cxfinal = rcWindow.right - rcWindow.left;
    int cyfinal = rcWindow.bottom - rcWindow.top;
    if (cx != cxfinal || cy != cyfinal)
    {
        Log(ERR, "Error: actual starting size (%i x %i) wasn't what we asked for.\n",
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

    GetWindowRect(hwnd, &rcWindow);
    GetClientRect(hwnd, &rcClient);
}

void CWindow::HandleMinMaxInfo(MINMAXINFO* pmmi)
{
    Log(GET_MINMAXINFO, "HANDLING WM_GETMINMAXINFO.\n");
    LogMINMAXINFO(GET_MINMAXINFO, pmmi);
}

void CWindow::HandleDPIChanged(UINT newdpi, RECT* prc)
{
    Log(DPI, "HANDLING WM_DPICHANGED.\n");
    Log(DPI, " - new DPI: %i\n", newdpi);
    Log(DPI, " - suggestion rect size: %i x %i\n",
        prc->right - prc->left, prc->bottom - prc->top);

    dpi = GetDpiForWindow(hwnd);
    if (dpi != newdpi)
    {
        Log(ERR, "WM_DPICHANGED passed DPI %i, but getting %i from GetDpiForWindow?\n", newdpi, dpi);
    }

    UINT dpiPrev = dpi;

    SetWindowPos(hwnd, nullptr,
        prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top,
        SWP_NOZORDER | SWP_NOACTIVATE);

    if (dpiPrev != dpi)
    {
        Log(ERR, "Suggestion rect in DPI change caused a DPI change!\n");
    }
}

void CWindow::HandleGetDpiScaledSize(UINT dpi, PSIZE pSize)
{
    Log(DPI, "HANDLING WM_GETDPISCALEDSIZE.\n");
    Log(DPI, " - proposed DPI: %i\n", dpi);
    Log(DPI, " - proposed size: %i x %i\n", pSize->cx, pSize->cy);

    // TODO muck with proposed size?

}

void CWindow::HandlePaint(HDC hdc)
{
    static HBRUSH hbr = CreateSolidBrush(rgbBackground);
    FillRect(hdc, &rcClient, hbr);

    // TODO UI to toggle log types

}

void CWindow::HandleButton(bool bLeft, bool bDown)
{
    Log(VERBOSE, "\n <%s button %s>\n\n\n",
        bLeft ? "left" : "right", bDown ? "down" : "up");

    // TODO UI to toggle log types

}
