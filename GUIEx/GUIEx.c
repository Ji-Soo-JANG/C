#include <windows.h>
#include <stdio.h>
#include "Loghelper.h"
#include <CommCtrl.h>


//로그용 패널
HWND hDebugPanel;
HWND hEditKanji, hEditKana, hEditMeaning, hEditExample, hButtonRegister;
HFONT hFont;

// パンネル構造体作成
struct Component {
    HWND hwnd;
    wchar_t name[50];
};

struct Component coms[3];
#define COMS_COUNT (sizeof(coms) / sizeof(coms[0]))

// placeholder 機能作成
void setPlaceholder(struct Component* com);


// 메시지 처리 함수 (닫기 이벤트만 처리)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    wchar_t debug[64];
    swprintf(debug, 64, L"[msg] 0x%X\n", msg);
    OutputDebugStringW(debug);  
    
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;


    switch (msg) {
    case WM_CREATE:
        // 日本語が表示できるフォントを作成
        hFont = CreateFontW(
            18, 0, 0, 0,
            FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"MS UI Gothic"
        );

        // デバッグパネルを作成
        hDebugPanel = CreateWindowW(
            L"STATIC", L"디버그 패널 초기화됨",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 10, 500, 20,
            hwnd, NULL, pcs->hInstance, NULL
        );

              // 画面の要素作成
        hEditKana = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 50, 200, 50, hwnd, (HMENU)1001, pcs->hInstance, NULL);
        hEditMeaning = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 100, 200, 50, hwnd, (HMENU)1002, pcs->hInstance, NULL);
        hEditExample = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 150, 200, 50, hwnd, (HMENU)1003, pcs->hInstance, NULL);
        hButtonRegister = CreateWindowW(L"Button", L"単語登録", WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 200, 200, 50, hwnd, (HMENU)1004, pcs->hInstance, NULL);
        
        struct Component cKana = { hEditKana, L"カナ" };
        struct Component cMeaning = { hEditMeaning, L"意味" };
        struct Component cExample = { hEditExample, L"例文" };

        coms[0] = cKana;
        coms[1] = cMeaning;
        coms[2] = cExample;
        
        SendMessageW(hEditKana, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hEditMeaning, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hEditExample, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hButtonRegister, WM_SETFONT, (WPARAM)hFont, TRUE);

        for (int i = 0; i < COMS_COUNT; i++) {
            setPlaceholder(&coms[i]);
        }
        break;


    case WM_COMMAND:
    {
        //OutputDebugStringW(L"[WM_COMMAND]");
        int code = HIWORD(wParam); 
        /*wchar_t log[128];
        wsprintfW(log, L"%d", code);
        OutputDebugStringW(log);*/

        if (code == EN_SETFOCUS) {
            for (int i = 0; i < COMS_COUNT; i++) {
                if ((HWND)lParam == coms[i].hwnd) {
                    wchar_t buf[128];
                    GetWindowTextW(lParam, buf, sizeof(buf) / sizeof(wchar_t));

                    wchar_t expected[128];
                    wsprintfW(expected, L"%sを入力してください", coms[i].name);
                    //OutputDebugString(expected);

                    if (wcscmp(buf, expected) == 0) {
                        SetWindowTextW(coms[i].hwnd, L"");
                    }
                }
            }
        }

        if (code == EN_KILLFOCUS) {
            for (int i = 0; i < COMS_COUNT; i++) {
                if ((HWND)lParam == coms[i].hwnd) {
                    wchar_t buf[128];
                    GetWindowTextW((HWND)lParam, buf, sizeof(buf) / sizeof(wchar_t));

                    if (wcscmp(buf,L"") == 0) {
                        setPlaceholder(&coms[i]);
                    }
                }
            }
        }
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

void setPlaceholder(struct Component* com) {
    const wchar_t* suffix = L"を入力してください";
    HWND hwnd = com->hwnd;
    wchar_t buf[128];
    wsprintfW(buf, L"%s%s", com->name, suffix);

    SetWindowTextW(com->hwnd, buf);
}

