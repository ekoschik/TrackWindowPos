
#include "stdafx.h"
#include "windows.h"
#include "header.h"

bool LOG_TYPE_MASK[LT_LAST - LT_FIRST + 1] = {};

void SetLogBit(LOG_TYPE lg, bool bSet)
{
    LOG_TYPE_MASK[lg] = bSet;
}

void Log(LOG_TYPE type, CHAR* format, ...)
{
    if (LOG_TYPE_MASK[type]) {
        //if (type == ERR) {
        //    // can i do red text for error? ...OutputDebugStringA(buffer);
        //    printf("ERROR: ");
        //}

        static CHAR buffer[1000];
        va_list argptr;
        va_start(argptr, format);
        vsprintf_s(buffer, format, argptr);
        va_end(argptr);

        printf((const char *)&buffer);        
    }
}

struct WINDOWPOSFLAG { UINT f; char* name; };

WINDOWPOSFLAG WPFLAGS[cWINDOWPOSFLAGS] = {
    { SWP_DRAWFRAME , "SWP_DRAWFRAME" },
    { SWP_FRAMECHANGED , "SWP_FRAMECHANGED" },
    { SWP_HIDEWINDOW , "SWP_HIDEWINDOW" },
    { SWP_NOACTIVATE , "SWP_NOACTIVATE" },
    { SWP_NOCOPYBITS , "SWP_NOCOPYBITS" },
    { SWP_NOMOVE , "SWP_NOMOVE" },
    { SWP_NOOWNERZORDER , "SWP_NOOWNERZORDER" },
    { SWP_NOREDRAW , "SWP_NOREDRAW" },
    { SWP_NOREPOSITION , "SWP_NOREPOSITION" },
    { SWP_NOSENDCHANGING , "SWP_NOSENDCHANGING" },
    { SWP_NOSIZE , "SWP_NOSIZE" },
    { SWP_NOZORDER , "SWP_NOZORDER" },
    { SWP_SHOWWINDOW , "SWP_SHOWWINDOW" }
};

bool IsWindowFlagPresent(UINT flags, int i, char* & pstrFlagName)
{
    if (flags & WPFLAGS[i].f)
    {
        pstrFlagName = WPFLAGS[i].name;
        return true;
    }
    pstrFlagName = nullptr;
    return false;
}

void LogWINDOWPOS(LOG_TYPE logtype, WINDOWPOS* pwp)
{
    Log(logtype, " - WINDOWPOS:\n");

    if (pwp->hwnd != nullptr)
    {
        Log(logtype, "    - hwnd: 0x%X\n", pwp->hwnd);
    }

    if (pwp->hwndInsertAfter != nullptr)
    {
        Log(logtype, "    - hwndInsertAfter: 0x%X\n", pwp->hwndInsertAfter);
    }

    if (!(pwp->flags & SWP_NOMOVE))
    {
        Log(logtype, "    - x: %i, y: %i\n", pwp->x, pwp->y);
    }

    if (!(pwp->flags & SWP_NOSIZE))
    {
        Log(logtype, "    - cx: %i, cy : %i\n", pwp->cx, pwp->cy);
    }

    bool bAnyFlags = false;
    char* pstrFlagName = nullptr;
    for (int i = 0; i < cWINDOWPOSFLAGS; i++)
    {
        if (IsWindowFlagPresent(pwp->flags, i, pstrFlagName))
        {
            Log(logtype, "    - %s\n", pstrFlagName);
            bAnyFlags = true;
        }
    }

    if (!bAnyFlags)
    {
        Log(logtype, "    - no flags\n");
    }
}

void LogMINMAXINFO(LOG_TYPE logtype, MINMAXINFO* pmmi)
{
    Log(logtype, " - MINMAXINFO:\n");
    Log(logtype, "    - ptReserved    : %i x %i\n", pmmi->ptReserved.x, pmmi->ptReserved.y);
    Log(logtype, "    - ptMaxSize     : %i x %i\n", pmmi->ptMaxSize.x, pmmi->ptMaxSize.y);
    Log(logtype, "    - ptMaxPosition : %i x %i\n", pmmi->ptMaxPosition.x, pmmi->ptMaxPosition.y);
    Log(logtype, "    - ptMinTrackSize: %i x %i\n", pmmi->ptMinTrackSize.x, pmmi->ptMinTrackSize.y);
    Log(logtype, "    - ptMaxTrackSize: %i x %i\n", pmmi->ptMaxTrackSize.x, pmmi->ptMaxTrackSize.y);
}

char* WMnames(UINT msg)
{
    switch (msg)
    {
    case WM_NCCREATE:           return "WM_NCCREATE";
    case WM_CREATE:             return "WM_CREATE";
    case WM_DPICHANGED:         return "WM_DPICHANGED";
    case WM_GETDPISCALEDSIZE:   return "WM_GETDPISCALEDSIZE";
    case WM_GETMINMAXINFO:      return "WM_GETMINMAXINFO";
    case WM_WINDOWPOSCHANGING:  return "WM_WINDOWPOSCHANGING";
    case WM_WINDOWPOSCHANGED:   return "WM_WINDOWPOSCHANGED";
    case WM_LBUTTONDOWN:        return "WM_LBUTTONDOWN";
    case WM_LBUTTONUP:          return "WM_LBUTTONUP";
    case WM_PAINT:              return "WM_PAINT";
    case WM_DESTROY:            return "WM_DESTROY";
    case WM_NCDESTROY:          return "WM_NCDESTROY";
    case WM_NCCALCSIZE:         return "WM_NCCALCSIZE";
    case WM_NCHITTEST:          return "WM_NCHITTEST";
    case WM_NCPAINT:            return "WM_NCPAINT";
    case WM_NCACTIVATE:         return "WM_NCACTIVATE";
    case WM_QUIT:               return "WM_QUIT";
    case WM_ERASEBKGND:         return "WM_ERASEBKGND";
    case WM_SYSCOLORCHANGE:     return "WM_SYSCOLORCHANGE";
    case WM_SHOWWINDOW:         return "WM_SHOWWINDOW";
    case WM_WININICHANGE:       return "WM_WININICHANGE";
    case WM_NULL:               return "WM_NULL";
    case WM_MOVE:               return "WM_MOVE";
    case WM_SIZE:               return "WM_SIZE";
    case WM_ACTIVATE:           return "WM_ACTIVATE";
    case WM_SETFOCUS:           return "WM_SETFOCUS";
    case WM_KILLFOCUS:          return "WM_KILLFOCUS";
    case WM_ENABLE:             return "WM_ENABLE";
    case WM_SETREDRAW:          return "WM_SETREDRAW";
    case WM_SETTEXT:            return "WM_SETTEXT";
    case WM_GETTEXT:            return "WM_GETTEXT";
    case WM_GETTEXTLENGTH:      return "WM_GETTEXTLENGTH";
    case WM_CLOSE:              return "WM_CLOSE";
    case WM_QUERYENDSESSION:    return "WM_QUERYENDSESSION";
    case WM_QUERYOPEN:          return "WM_QUERYOPEN";
    case WM_ENDSESSION:         return "WM_ENDSESSION";
    case WM_ACTIVATEAPP:        return "WM_ACTIVATEAPP";
    case WM_GETICON:            return "WM_GETICON";
    case WM_IME_SETCONTEXT:     return "WM_IME_SETCONTEXT";
    case WM_IME_NOTIFY:         return "WM_IME_NOTIFY";






    }
    
    static CHAR buffer[1000];
    sprintf_s(buffer, "unknown msg 0x%X", msg);
    return (char*)&buffer;
}
