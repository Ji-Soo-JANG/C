#include <windows.h>
#include <stdio.h>
#include "Loghelper.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib") 
#include "ui.h"


// main proc
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
    case WM_CREATE: {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        ui_on_create(hwnd, pcs);      
        return 0;
    }

    case WM_COMMAND: {
        int code = HIWORD(wParam);
        int id = LOWORD(wParam);
        HWND ctl = (HWND)lParam;

        ui_on_command(hwnd, id, code, ctl);  
        return 0;
    }

    case WM_NOTIFY: {
        LRESULT result = 0;
        if (ui_on_notify(hwnd, wParam, lParam, &result)) {
            return result;
        }
        break;  
    }

    case WM_DESTROY:
        ui_on_destroy();           
        PostQuitMessage(0);
        return 0;
    
     //debug pannel
    default: {
        const wchar_t* name = GetMessageName(msg);
        if (name) {
            SetWindowTextW(debug_panel, name);
        }
        break;
    }
}

return DefWindowProc(hwnd, msg, wParam, lParam);
}

// EidtProc
LRESULT CALLBACK EditWndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    switch (msg) {
    case WM_CREATE:
        //MessageBox(hwnd, L"ok", "", MB_OK);
        ui_word_on_create(hwnd, (LPCREATESTRUCT)l_param);
        return 0;

    case WM_COMMAND: {
        int code = HIWORD(w_param);
        int id = LOWORD(w_param);
        HWND hCtl = (HWND)l_param;

        ui_word_edit_on_command(hwnd, id, code, hCtl);
        return 0;
    }

    case WM_DESTROY:
        return 0;
    }

    return DefWindowProcW(hwnd, msg, w_param, l_param);
}




// 프로그램 시작점 (GUI용)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {

    // 메인 창 클래스
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"BasicWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    // 수정 창 클래스
    WNDCLASSW wc_edit = { 0 };
    wc_edit.lpfnWndProc = EditWndProc;
    wc_edit.hInstance = hInstance;
    wc_edit.lpszClassName = L"EditWindowClass";
    wc_edit.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc_edit)) {
        MessageBoxW(NULL, L"EditWindowClass 등록 실패", L"Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    
    // 창 생성
    HWND hwnd = CreateWindow(
        L"BasicWindow",           // 클래스 이름
        L"日本語単語暗記",            // 창 제목
        WS_OVERLAPPEDWINDOW,     // 창 스타일
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        NULL, NULL, hInstance, NULL
    );

    // リストビュー
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES;

    if (!InitCommonControlsEx(&icex)) {
        MessageBox(NULL, L"공통 컨트롤 초기화 실패", L"오류", MB_ICONERROR);
        return FALSE;
    }

    ShowWindow(hwnd, nCmdShow);

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(hwnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}




