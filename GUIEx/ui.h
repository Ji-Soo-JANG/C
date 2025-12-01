// ui.h
#pragma once
#include <windows.h>

struct registerCtrs {
    HWND hwnd;
    const wchar_t* name;
};

// main 쪽에서 쓰는 것만 extern
extern HWND hDebugPanel;
LRESULT CALLBACK EditWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// WndProc에서 호출할 UI 엔트리 포인트
void UI_OnCreate(HWND hwnd, LPCREATESTRUCT pcs);
void UI_OnCommand(HWND hwnd, int id, int code, HWND hwndCtl);
BOOL UI_OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam, LRESULT* result);
void UI_OnDestroy(void);

void UI_WordEdit_OnCreate(HWND hwnd, LPCREATESTRUCT pcs);
void UI_WordEdit_OnCommand(HWND hwnd, int id, int code, HWND hwndCtl);
