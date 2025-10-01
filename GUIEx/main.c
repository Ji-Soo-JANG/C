#include <windows.h>
#include <stdio.h>
#include "Loghelper.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib") 
#include "ui.h"


//HWND hBtnSearch, hListViewType, hListViewWord;

// 메시지 처리 함수 (닫기 이벤트만 처리)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    /*wchar_t debug[64];
    swprintf(debug, 64, L"[msg] 0x%X\n", msg);
    OutputDebugStringW(debug);  
    */
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;


    switch (msg) {
    case WM_CREATE:
        // 日本語が表示できるフォントを作成
        createHFont();

        // デバッグパネルを作成
        createDebugPanel(hwnd, pcs);

        // ホームのボタン作成
        createBtnHome(hwnd, pcs);

        // 格の機能のボタン作成
        createBtnMains(hwnd, pcs);

        // 登録画面の要素作成
        createRegCtrs(hwnd, pcs);

        // 照会画面の要素作成
        createSearchCtrs(hwnd, pcs);

        break;


    case WM_COMMAND:
    {
        int code = HIWORD(wParam); 
        int id = LOWORD(wParam);
    /*  wchar_t log[128];
        wsprintfW(log, L"[WM_COMMAND] id=%d, code=%d\n", id, code);
        OutputDebugStringW(log);
     */
        if (lParam == NULL) {
            OutputDebugStringW(L"→ lParam is NULL (maybe menu or accelerator)\n");
        }
        if(code == BN_CLICKED)
            clickEvent(hwnd, lParam, code, id);

        if(code == EN_SETFOCUS)
            setFocusEvent(lParam);

        if (code == EN_KILLFOCUS)
            killFocusEvent(lParam);
    }
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
        if (hFont) {
            DeleteObject(hFont);
            hFont = NULL;
        }
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
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}




