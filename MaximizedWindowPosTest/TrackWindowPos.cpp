
 
#include "stdafx.h"
#include "windows.h"
#include "header.h"
#include <map>

using namespace std;

map<HWND, CWindow*> WindowMap;

void InitLogMask()
{
    SetLogBit(ERR, true);
    //SetLogBit(MOUSE_BUTTON, true);

    //SetLogBit(MONITOR_CHANGE, true);
    //SetLogBit(DPI, true);
    
    //SetLogBit(WINDOW_POS_CHANGE, true);
    //SetLogBit(POS_CHANGING, true);
    SetLogBit(POS_CHANGED, true);
    //SetLogBit(GET_MINMAXINFO, true);
    //SetLogBit(CREATE, true);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CREATE)
    {
        WindowMap[hwnd] = new CWindow(hwnd);
    }

    CWindow* pwnd = WindowMap[hwnd];

    if (pwnd == nullptr)
    {
        // Pass everything before WM_CREATE straight to DefWndProc
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    pwnd->SetCurrentMessage(message);

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

    case WM_NCPAINT:
        pwnd->HandleNCPaint();
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        // clicks create spaces in message spew
        Log(MOUSE_BUTTON, "\n%s\n\n", (message == WM_LBUTTONDOWN) ? "WM_LBUTTONDOWN" : "WM_LBUTTONUP");
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

    pwnd->CheckWindowState();

    return DefWindowProc(hwnd, message, wParam, lParam);
}

int main()
{
    HackSetThreadToPMV2();
    InitLogMask();

    LPCWSTR szClass = L"MaxWindowPosTestClass";
    LPCWSTR szTitle = L"MaxWindowPosTestTitle";
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
        Log(ERR, "RegisterWindow failed, bailing...\n");
        return NULL;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

