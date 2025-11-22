#include "ui.h"
#include "dictionary.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

HWND hDebugPanel;
HFONT hFont;  
HWND hBtnHome;
HWND hBtnRegisterPage, hBtnSearchPage, hBtnQuizPage;
HWND hEditKanji, hEditKana, hEditMeaning, hEditExample, hBtnRegister;
HWND hBtnSearch, hListViewType, hListViewWord;

HWND mainHWNDs[3];

struct registerCtrs regStructs[4];
HWND regHWNDs[5];
HWND searchHWNDs[2];

#define countMainCtrs (sizeof(mainHWNDs) / sizeof(mainHWNDs[0]))
#define countRegEdits (sizeof(regStructs) / sizeof(regStructs[0]))
#define countRegCtrs (sizeof(regHWNDs) / sizeof(regHWNDs[0]))
#define countSearchCtrs (sizeof(searchHWNDs) / sizeof(searchHWNDs[0]))

static BOOL readText(HWND h, wchar_t* out, int cap) {
    wchar_t tmp[512];
    GetWindowTextW(h, tmp, _countof(tmp));
    if (wcsstr(tmp, L"を入力してください")) { out[0] = L'\0'; return TRUE; }
    wcsncpy_s(out, cap, tmp, _TRUNCATE);
    return TRUE;
}

void createHFont() {
    hFont = CreateFontW(
        18, 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH, L"MS UI Gothic"
    );
}

void createDebugPanel(HWND hwnd, LPCREATESTRUCT pcs) {
    hDebugPanel = CreateWindowW(
        L"STATIC", L"디버그 패널 초기화됨",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        10, 10, 500, 20,
        hwnd, NULL, pcs->hInstance, NULL
    );
}

void createBtnHome(HWND hwnd, LPCREATESTRUCT pcs) {
    hBtnHome = CreateWindowW(L"Button", L"ホームへ", WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 500, 100, 50, hwnd, (HMENU)1001, pcs->hInstance, NULL);
    SendMessageW(hBtnHome, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void createBtnMains(HWND hwnd, LPCREATESTRUCT pcs) {
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
}

void createRegCtrs(HWND hwnd, LPCREATESTRUCT pcs) {
    hEditKanji = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 50, 200, 50, hwnd, (HMENU)1005, pcs->hInstance, NULL);
    hEditKana = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 100, 200, 50, hwnd, (HMENU)1006, pcs->hInstance, NULL);
    hEditMeaning = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 150, 200, 50, hwnd, (HMENU)1007, pcs->hInstance, NULL);
    hEditExample = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 200, 200, 50, hwnd, (HMENU)1008, pcs->hInstance, NULL);
    hBtnRegister = CreateWindowW(L"Button", L"単語登録", WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 250, 200, 50, hwnd, (HMENU)1009, pcs->hInstance, NULL);

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

}

void createSearchCtrs(HWND hwnd, LPCREATESTRUCT pcs) {
    hListViewType = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, 30, 50, 100, 450, hwnd, (HMENU)2001, pcs->hInstance, NULL);
    LVCOLUMNW colType = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    colType.cx = 100;  colType.pszText = L"リスト";     ListView_InsertColumn(hListViewType, 0, &colType);

    hListViewWord = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, 130, 50, 600, 450, hwnd, (HMENU)2002, pcs->hInstance, NULL);
    LVCOLUMNW colWord = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    colWord.cx = 150; colWord.pszText = L"漢字";      ListView_InsertColumn(hListViewWord, 0, &colWord);
    colWord.cx = 150; colWord.pszText = L"カナ";      ListView_InsertColumn(hListViewWord, 0, &colWord);
    colWord.cx = 150; colWord.pszText = L"意味";      ListView_InsertColumn(hListViewWord, 0, &colWord);
    colWord.cx = 150; colWord.pszText = L"例文";      ListView_InsertColumn(hListViewWord, 0, &colWord);

    searchHWNDs[0] = hListViewType;
    searchHWNDs[1] = hListViewWord;

    toggleWindow(searchHWNDs, countSearchCtrs, FALSE);
}

// placeholder 機能作成
void setPlaceholder(struct registerCtrs* com) {
    const wchar_t* suffix = L"を入力してください";
    HWND hwnd = com->hwnd;
    wchar_t buf[128];
    wsprintfW(buf, L"%s%s", com->name, suffix);

    SetWindowTextW(com->hwnd, buf);
}

// toggleShow 機能作成
void toggleWindow(HWND arr[], int count, BOOL show) {
    wchar_t dbg[64];
    for (int i = 0; i < count; i++) {
        wsprintfW(dbg, L"toggleWindow: HWND=0x%p, show=%d\n", arr[i], show);
        OutputDebugStringW(dbg);

        ShowWindow(arr[i], show ? SW_SHOW : SW_HIDE);
    }
}

void clickEvent(HWND hwnd, LPARAM lParam, int code, int id) {
    if (code == BN_CLICKED) {
        wchar_t log[128];
        HWND ctr = (HWND)lParam;
                
        wchar_t btnName[128];
        GetWindowTextW(ctr, btnName, _countof(btnName));
        wsprintfW(log, L"%s가 눌렸습니다.\n", btnName);
        OutputDebugStringW(log);

        const Word* words;

        switch (id) {
            case 1001 :
                toggleWindow(mainHWNDs, countMainCtrs, TRUE);
                toggleWindow(regHWNDs, countRegCtrs, FALSE);
                toggleWindow(searchHWNDs, countSearchCtrs, FALSE);
                break;

            case 1002 :
                toggleWindow(mainHWNDs, countMainCtrs, FALSE);
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);

                toggleWindow(regHWNDs, countRegCtrs, TRUE);
                for (int i = 0; i < countRegEdits; i++) {
                    setPlaceholder(&regStructs[i]);
                }
                break;

            case 1003 :
                toggleWindow(mainHWNDs, countMainCtrs, FALSE);
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
                toggleWindow(searchHWNDs, countSearchCtrs, TRUE);

                words = dict_get_all();
                size_t count = dict_count();
                wchar_t buf[64];
                wsprintfW(buf, L"count:%d", count);

                MessageBoxW(hwnd, count, L"count", MB_OK | MB_ICONINFORMATION);
                break;

            case 1004 :
                toggleWindow(mainHWNDs, countMainCtrs, FALSE);
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);

                toggleWindow(searchHWNDs, 1, TRUE);
                break;

            case 1009:
            {
                Word w = (Word){ 0 };
                readText(hEditKanji, w.kanji, _countof(w.kanji));
                readText(hEditKana, w.kana, _countof(w.kana));
                readText(hEditMeaning, w.meaning, _countof(w.meaning));
                readText(hEditExample, w.example, _countof(w.example));
                w.type = 0; w.proficiency = 0;

                size_t rc = dict_add(&w);
                if (rc == 0) {
                    int n = dict_count();
                    wchar_t ok[160];
                    wsprintfW(ok, L"登録しました。（現在 %d 件）", n);
                    MessageBoxW(hwnd, ok, L"OK", MB_OK | MB_ICONINFORMATION);

                    wchar_t dbg[256]; wsprintfW(dbg, L"[登録OK] count=%d, last=%s / %s\n", n, w.kanji, w.kana);
                    MessageBoxW(hwnd, dbg, L"登録成功", MB_OK | MB_ICONINFORMATION);
                
                    dict_save();
                }
            }
                break;
        }

    }
}

void setFocusEvent(LPARAM lParam) {
    for (int i = 0; i < countRegEdits; i++) {
        if ((HWND)lParam == regStructs[i].hwnd) {
            wchar_t buf[128];
            GetWindowTextW((HWND)lParam, buf, _countof(buf));

            wchar_t expected[128];
            wsprintfW(expected, L"%sを入力してください", regStructs[i].name);
            //OutputDebugString(expected);

            if (wcscmp(buf, expected) == 0) {
                SetWindowTextW(regStructs[i].hwnd, L"");
            }
        }
    }
}

void killFocusEvent(LPARAM lParam) {
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