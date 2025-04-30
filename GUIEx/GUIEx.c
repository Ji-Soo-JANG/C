#include <windows.h>
#include <stdio.h>
#include "Loghelper.h"
#include <CommCtrl.h>


//로그용 패널
HWND hDebugPanel;
HWND hBtnHome;
HWND hBtnRegisterPage, hBtnSearchPage, hBtnQuizPage;
HWND hEditKanji, hEditKana, hEditMeaning, hEditExample, hBtnRegister;
HFONT hFont;

// ホームのパンネル配列
HWND mainHWNDs[3];
#define countMainCtrs (sizeof(mainHWNDs) / sizeof(mainHWNDs[0]))

// パンネル構造体作成
struct registerCtrs {
    HWND hwnd;
    wchar_t name[50];
};

struct registerCtrs regStructs[4];
HWND regHWNDs[5];
#define countRegEdits (sizeof(regStructs) / sizeof(regStructs[0]))
#define countRegCtrs (sizeof(regHWNDs) / sizeof(regHWNDs[0]))


// toggleShow 機能作成
void toggleWindow(HWND arr[], int count, BOOL show);

// placeholder 機能作成
void setPlaceholder(struct registerCtrs* com);


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

        // ホームのボタン作成
        hBtnHome = CreateWindowW(L"Button", L"ホームへ", WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 500, 100, 50, hwnd, (HWND)1001, pcs->hInstance, NULL);
        SendMessageW(hBtnHome, WM_SETFONT, (WPARAM)hFont, TRUE);

        // 格の機能のボタン作成
        hBtnRegisterPage = CreateWindowW(L"Button", L"登録", WS_CHILD | WS_VISIBLE | WS_BORDER, 260, 100, 100, 200, hwnd, (HMENU)1002, pcs->hInstance, NULL);
        hBtnSearchPage = CreateWindowW(L"Button", L"照会", WS_CHILD | WS_VISIBLE | WS_BORDER, 360, 100, 100, 200, hwnd, (HMENU)1003, pcs->hInstance, NULL);
        hBtnQuizPage = CreateWindowW(L"Button", L"クイズ", WS_CHILD | WS_VISIBLE | WS_BORDER, 460, 100, 100, 200, hwnd, (HMENU)1004, pcs->hInstance, NULL);
        SendMessageW(hBtnRegisterPage, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hBtnSearchPage, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hBtnQuizPage, WM_SETFONT, (WPARAM)hFont, TRUE);

        mainHWNDs[0] = hBtnRegisterPage;
        mainHWNDs[1] = hBtnSearchPage;
        mainHWNDs[2] = hBtnQuizPage;

        toggleWindow(mainHWNDs, countMainCtrs, TRUE);



        // 画面の要素作成
        hEditKanji = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 50, 200, 50, hwnd, (HMENU)1005, pcs->hInstance, NULL);
        hEditKana = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 100, 200, 50, hwnd, (HMENU)1006, pcs->hInstance, NULL);
        hEditMeaning = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 150, 200, 50, hwnd, (HMENU)1007, pcs->hInstance, NULL);
        hEditExample = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 200, 200, 50, hwnd, (HMENU)1008, pcs->hInstance, NULL);
        hBtnRegister = CreateWindowW(L"Button", L"単語登録", WS_CHILD | WS_VISIBLE | WS_BORDER , 50, 250, 200, 50, hwnd, (HMENU)1009, pcs->hInstance, NULL);
        
        struct registerCtrs cKanji = { hEditKanji, L"漢字" };
        struct registerCtrs cKana = { hEditKana, L"カナ" };
        struct registerCtrs cMeaning = { hEditMeaning, L"意味" };
        struct registerCtrs cExample = { hEditExample, L"例文" };

        regStructs[0] = cKanji;
        regStructs[1] = cKana;
        regStructs[2] = cMeaning;
        regStructs[3] = cExample;
        
        SendMessageW(hEditKanji, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hEditKana, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hEditMeaning, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hEditExample, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessageW(hBtnRegister, WM_SETFONT, (WPARAM)hFont, TRUE);

        for (int i = 0; i < countRegEdits; i++) {
            setPlaceholder(&regStructs[i]);
        }

        regHWNDs[0] = hEditKanji;
        regHWNDs[1] = hEditKana;
        regHWNDs[2] = hEditMeaning;
        regHWNDs[3] = hEditExample;
        regHWNDs[4] = hBtnRegister;

        toggleWindow(regHWNDs, countRegCtrs, FALSE);

        break;


    case WM_COMMAND:
    {
        //OutputDebugStringW(L"[WM_COMMAND]");
        int code = HIWORD(wParam); 
        /*wchar_t log[128];
        wsprintfW(log, L"%d", code);
        OutputDebugStringW(log);*/

        if (code == BN_CLICKED) {
            int id = LOWORD(wParam);
            
            wchar_t log[128];
            HWND ctr = (HWND)lParam;
            if (id == 1001) {
                toggleWindow(mainHWNDs, countMainCtrs, TRUE);
                toggleWindow(regHWNDs, countRegCtrs, FALSE);
             
            }

            for (int i = 0; i < countMainCtrs; i++) {
                if (ctr == mainHWNDs[i]) {
                    wchar_t btnName[128];
                    GetWindowTextW(ctr, btnName, _countof(btnName));
                    wsprintfW(log, L"%s가 눌렸습니다.\n", btnName);
                    OutputDebugStringW(log);

                    toggleWindow(mainHWNDs, countMainCtrs, FALSE);
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);

                    toggleWindow(regHWNDs, countRegCtrs, TRUE);
                    for (int i = 0; i < countRegEdits; i++) {
                        setPlaceholder(&regStructs[i]);
                    }

                    break;
                }
            }

        }


        if (code == EN_SETFOCUS) {
            for (int i = 0; i < countRegEdits; i++) {
                if ((HWND)lParam == regStructs[i].hwnd) {
                    wchar_t buf[128];
                    GetWindowTextW(lParam, buf, sizeof(buf) / sizeof(wchar_t));

                    wchar_t expected[128];
                    wsprintfW(expected, L"%sを入力してください", regStructs[i].name);
                    //OutputDebugString(expected);

                    if (wcscmp(buf, expected) == 0) {
                        SetWindowTextW(regStructs[i].hwnd, L"");
                    }
                }
            }
        }

        if (code == EN_KILLFOCUS) {
            for (int i = 0; i < countRegEdits; i++) {
                if ((HWND)lParam == regStructs[i].hwnd) {
                    wchar_t buf[128];
                    GetWindowTextW((HWND)lParam, buf, sizeof(buf) / sizeof(wchar_t));

                    if (wcscmp(buf,L"") == 0) {
                        setPlaceholder(&regStructs[i]);
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

    ShowWindow(hwnd, nCmdShow);

    // 메시지 루프
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void toggleWindow(HWND arr[], int count,  BOOL show) {
    wchar_t dbg[64];
    for (int i = 0; i < count; i++) {
        wsprintfW(dbg, L"toggleWindow: HWND=0x%p, show=%d\n", arr[i], show);
        OutputDebugStringW(dbg);

        ShowWindow(arr[i], show ? SW_SHOW : SW_HIDE);
    }
}

void setPlaceholder(struct registerCtrs* com) {
    const wchar_t* suffix = L"を入力してください";
    HWND hwnd = com->hwnd;
    wchar_t buf[128];
    wsprintfW(buf, L"%s%s", com->name, suffix);

    SetWindowTextW(com->hwnd, buf);
}

