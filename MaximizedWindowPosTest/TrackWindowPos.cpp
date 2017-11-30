
 
#include "stdafx.h"
#include "windows.h"
#include "header.h"
#include <map>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static std::map<HWND, CWindow*> WindowMap;
    if (message == WM_CREATE)
    {
        WindowMap[hwnd] = new CWindow(hwnd);
    }

    CWindow* pwnd = WindowMap[hwnd];
    if (pwnd == nullptr)
    {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    switch (message)
    {
    case WM_CREATE:
        pwnd->HandleCreate();
        break;

    case WM_DPICHANGED:
        pwnd->HandleDPIChanged(HIWORD(wParam), (RECT*)lParam);
        break;

    case WM_GETDPISCALEDSIZE:
        pwnd->HandleGetDpiScaledSize((UINT)wParam, (PSIZE)lParam);
        break;

    case WM_GETMINMAXINFO:
        pwnd->HandleMinMaxInfo((MINMAXINFO*)lParam);
        break;

    case WM_WINDOWPOSCHANGING:
        pwnd->HandlePosChanging((WINDOWPOS*)lParam);
        break;

    case WM_WINDOWPOSCHANGED:
        pwnd->HandlePosChanged((WINDOWPOS*)lParam);
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        pwnd->HandleButton(
            ((message == WM_LBUTTONDOWN) || (message == WM_LBUTTONUP)),
            ((message == WM_LBUTTONDOWN) || (message == WM_RBUTTONDOWN)));
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        pwnd->HandlePaint(hdc);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    
    return DefWindowProc(hwnd, message, wParam, lParam);
}

int main()
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    SetLogBit(ERR, true);
    SetLogBit(VERBOSE, true);
    SetLogBit(POS_CHANGING, true);
    SetLogBit(POS_CHANGED, true);
    SetLogBit(GET_MINMAXINFO, true);
    SetLogBit(DPI, true);

    LPCWSTR szClass = L"TrackWindowPos-MainWindow";
    LPCWSTR szTitle = L"TrackWindowPos";
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = szClass;

    if (!RegisterClassExW(&wcex)) {
        Log(ERR, "RegisterWindow failed, bailing...\n");
        return 1;
    }

    HWND hwnd = CreateWindow(
        szClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,0,0,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
    {
        Log(ERR, "CreateWindow failed, bailing...\n");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

