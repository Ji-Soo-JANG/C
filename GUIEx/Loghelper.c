
// loghelper.c
#include "loghelper.h"

const wchar_t* GetMessageName(UINT msg) {
    switch (msg) {
    case WM_CREATE: return L"WM_CREATE";
    case WM_DESTROY: return L"WM_DESTROY";
    case WM_MOVE: return L"WM_MOVE";
    case WM_SIZE: return L"WM_SIZE";
    case WM_PAINT: return L"WM_PAINT";
    case WM_KEYDOWN: return L"WM_KEYDOWN";
    case WM_KEYUP: return L"WM_KEYUP";
    case WM_CHAR: return L"WM_CHAR";
    case WM_MOUSEMOVE: return L"WM_MOUSEMOVE";
    case WM_LBUTTONDOWN: return L"WM_LBUTTONDOWN";
    case WM_RBUTTONDOWN: return L"WM_RBUTTONDOWN";
    case WM_COMMAND: return L"WM_COMMAND";
    case WM_SETFOCUS: return L"WM_SETFOCUS";
    case WM_KILLFOCUS: return L"WM_KILLFOCUS";
    }
    return NULL;
}
