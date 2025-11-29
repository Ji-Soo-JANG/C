#include <windows.h>
#include <stdio.h>
#include "Loghelper.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib") 
#include "ui.h"


//HWND hBtnSearch, hListViewType, hListViewWord;

// 메시지 처리 함수 (닫기 이벤트만 처리)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
    case WM_CREATE: {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        UI_OnCreate(hwnd, pcs);      
        return 0;
    }

    case WM_COMMAND: {
        int code = HIWORD(wParam);
        int id = LOWORD(wParam);
        HWND hCtl = (HWND)lParam;

        UI_OnCommand(hwnd, id, code, hCtl);  
        return 0;
    }

    case WM_NOTIFY: {
        LRESULT result = 0;
        if (UI_OnNotify(hwnd, wParam, lParam, &result)) {
            return result;
        }
        break;  
    }

    case WM_DESTROY:
        UI_OnDestroy();           
        PostQuitMessage(0);
        return 0;
    
     //debug pannel
    default: {
        const wchar_t* name = GetMessageName(msg);
        if (name) {
            SetWindowTextW(hDebugPanel, name);
        }
        break;
    }
}

return DefWindowProc(hwnd, msg, wParam, lParam);
}


// 프로그램 시작점 (GUI용)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {

    // 창 등록용 최소 설정
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"BasicWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);


    RegisterClass(&wc);

    // 창 생성
    HWND hwnd = CreateWindow(
        L"BasicWindow",           // 클래스 이름
        L"빈 창입니다",            // 창 제목
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




