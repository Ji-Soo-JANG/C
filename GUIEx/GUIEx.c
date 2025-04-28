#include <windows.h>
#include <stdio.h>
#include "Loghelper.h"


//로그용 패널
HWND hDebugPanel;



// 메시지 처리 함수 (닫기 이벤트만 처리)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    wchar_t debug[64];
    swprintf(debug, 64, L"[msg] 0x%X\n", msg);
    OutputDebugStringW(debug);  // Visual Studio Output 창에 출력됨
    
    switch (msg) {
    case WM_CREATE:
        hDebugPanel = CreateWindowW(
            L"STATIC", L"디버그 패널 초기화됨",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 10, 500, 20,
            hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );
        break;

    default:
        {
        const wchar_t* name = GetMessageName(msg);
        if (name){
            SetWindowTextW(hDebugPanel, name);
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

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

    RegisterClass(&wc);

    // 창 생성
    HWND hwnd = CreateWindow(
        L"BasicWindow",           // 클래스 이름
        L"빈 창입니다",            // 창 제목
        WS_OVERLAPPEDWINDOW,     // 창 스타일
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
