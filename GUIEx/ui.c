#include "ui.h"
#include "dictionary.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

// ===== 전역 UI 핸들 =====
HWND hDebugPanel;
HFONT hFont;

HWND hBtnHome;
HWND hBtnRegisterPage, hBtnSearchPage, hBtnQuizPage;
HWND hEditKanji, hEditKana, hEditMeaning, hEditExample, hBtnRegister;
HWND hBtnSearch, hListViewType, hListViewWord;

static HWND mainHWNDs[3];
static struct registerCtrs regStructs[4];
static HWND regHWNDs[5];
static HWND searchHWNDs[2];

#define countMainCtrs   (sizeof(mainHWNDs)  / sizeof(mainHWNDs[0]))
#define countRegEdits   (sizeof(regStructs) / sizeof(regStructs[0]))
#define countRegCtrs    (sizeof(regHWNDs)   / sizeof(regHWNDs[0]))
#define countSearchCtrs (sizeof(searchHWNDs)/ sizeof(searchHWNDs[0]))

// ===== 내부 헬퍼 함수 선언 (ui.c 전용) =====
static BOOL readText(HWND h, wchar_t* out, int cap);
static void setPlaceholder(struct registerCtrs* com);
static void toggleWindow(HWND arr[], int count, BOOL show);
static void on_button_click(HWND hwnd, int id, HWND ctr);
static void on_edit_setfocus(HWND hwndCtl);
static void on_edit_killfocus(HWND hwndCtl);
static void show_lists(wchar_t* lists);
static void show_words(void);
static void add_word(HWND hwnd);
static BOOL listview_contains(HWND hList, const wchar_t* text);

// ====== 공용 엔트리 포인트 (main.c → ui.c로 위임) ======

void UI_OnCreate(HWND hwnd, LPCREATESTRUCT pcs)
{
    // 폰트 생성
    hFont = CreateFontW(
        18, 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH, L"MS UI Gothic"
    );

    // 디버그 패널
 /*   hDebugPanel = CreateWindowW(
        L"STATIC", L"디버그 패널 초기화됨",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        10, 10, 500, 20,
        hwnd, NULL, pcs->hInstance, NULL
    );*/

    // 홈 버튼
    hBtnHome = CreateWindowW(
        L"Button", L"ホームへ",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        30, 500, 100, 50,
        hwnd, (HMENU)1001, pcs->hInstance, NULL
    );
    SendMessageW(hBtnHome, WM_SETFONT, (WPARAM)hFont, TRUE);

    // 메인 기능 버튼
    hBtnRegisterPage = CreateWindowW(L"Button", L"登録", WS_CHILD | WS_VISIBLE | WS_BORDER,
        260, 100, 100, 200, hwnd, (HMENU)1002, pcs->hInstance, NULL);
    hBtnSearchPage = CreateWindowW(L"Button", L"照会", WS_CHILD | WS_VISIBLE | WS_BORDER,
        360, 100, 100, 200, hwnd, (HMENU)1003, pcs->hInstance, NULL);
    hBtnQuizPage = CreateWindowW(L"Button", L"クイズ", WS_CHILD | WS_VISIBLE | WS_BORDER,
        460, 100, 100, 200, hwnd, (HMENU)1004, pcs->hInstance, NULL);

    SendMessageW(hBtnRegisterPage, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hBtnSearchPage, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hBtnQuizPage, WM_SETFONT, (WPARAM)hFont, TRUE);

    mainHWNDs[0] = hBtnRegisterPage;
    mainHWNDs[1] = hBtnSearchPage;
    mainHWNDs[2] = hBtnQuizPage;
    toggleWindow(mainHWNDs, countMainCtrs, TRUE);

    // 등록 화면 컨트롤
    hEditKanji = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 50, 200, 50, hwnd, (HMENU)1005, pcs->hInstance, NULL);
    hEditKana = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 100, 200, 50, hwnd, (HMENU)1006, pcs->hInstance, NULL);
    hEditMeaning = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 150, 200, 50, hwnd, (HMENU)1007, pcs->hInstance, NULL);
    hEditExample = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 200, 200, 50, hwnd, (HMENU)1008, pcs->hInstance, NULL);
    hBtnRegister = CreateWindowW(L"Button", L"単語登録", WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 250, 200, 50, hwnd, (HMENU)1009, pcs->hInstance, NULL);

    struct registerCtrs cKanji = { hEditKanji,   L"漢字" };
    struct registerCtrs cKana = { hEditKana,    L"カナ" };
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

    // 조회 화면 컨트롤
    hListViewType = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS,
        30, 50, 100, 450, hwnd, (HMENU)2001, pcs->hInstance, NULL);

    LVCOLUMNW colType = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    colType.cx = 100;
    colType.pszText = L"リスト";
    ListView_InsertColumn(hListViewType, 0, &colType);

    hListViewWord = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS,
        130, 50, 600, 450, hwnd, (HMENU)2002, pcs->hInstance, NULL);

    LVCOLUMNW colWord = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    colWord.cx = 150; colWord.pszText = L"漢字";  ListView_InsertColumn(hListViewWord, 0, &colWord);
    colWord.cx = 150; colWord.pszText = L"カナ";  ListView_InsertColumn(hListViewWord, 1, &colWord);
    colWord.cx = 150; colWord.pszText = L"意味";  ListView_InsertColumn(hListViewWord, 2, &colWord);
    colWord.cx = 150; colWord.pszText = L"例文";  ListView_InsertColumn(hListViewWord, 3, &colWord);

    searchHWNDs[0] = hListViewType;
    searchHWNDs[1] = hListViewWord;
    toggleWindow(searchHWNDs, countSearchCtrs, FALSE);

    // 사전 초기화
    dict_init();
}

void UI_OnCommand(HWND hwnd, int id, int code, HWND hwndCtl)
{
    if (code == BN_CLICKED) {
        on_button_click(hwnd, id, hwndCtl);
    }
    else if (code == EN_SETFOCUS) {
        on_edit_setfocus(hwndCtl);
    }
    else if (code == EN_KILLFOCUS) {
        on_edit_killfocus(hwndCtl);
    }
}

BOOL UI_OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
    LPNMHDR pNMHDR = (LPNMHDR)lParam;

    // 리스트 이름 리스트뷰에서 온 통지인지
    if (pNMHDR->hwndFrom == hListViewType) {

        if (pNMHDR->code == LVN_ITEMCHANGED) {
            NMLISTVIEW* pNMLV = (NMLISTVIEW*)lParam;

            if ((pNMLV->uChanged & LVIF_STATE) &&
                (pNMLV->uNewState & LVIS_SELECTED) &&
                !(pNMLV->uOldState & LVIS_SELECTED)) {

                int index = pNMLV->iItem;
                if (index >= 0) {
                    wchar_t buf[256];
                    ListView_GetItemText(hListViewType, index, 0, buf, 256);
                    show_words();

                }
            }

            *result = 0;
            return TRUE;
        }
    }

    return FALSE;
}

void UI_OnDestroy(void)
{
    dict_save();
    dict_shutdown();

    if (hFont) {
        DeleteObject(hFont);
        hFont = NULL;
    }
}

// ===== 내부 헬퍼 구현부 =====

static BOOL readText(HWND h, wchar_t* out, int cap)
{
    wchar_t tmp[512];
    GetWindowTextW(h, tmp, _countof(tmp));
    if (wcsstr(tmp, L"を入力してください")) {
        out[0] = L'\0';
        return TRUE;
    }
    wcsncpy_s(out, cap, tmp, _TRUNCATE);
    return TRUE;
}

static void setPlaceholder(struct registerCtrs* com)
{
    const wchar_t* suffix = L"を入力してください";
    wchar_t buf[128];
    wsprintfW(buf, L"%s%s", com->name, suffix);
    SetWindowTextW(com->hwnd, buf);
}

static void toggleWindow(HWND arr[], int count, BOOL show)
{
    wchar_t dbg[64];
    for (int i = 0; i < count; i++) {
        wsprintfW(dbg, L"toggleWindow: HWND=0x%p, show=%d\n", arr[i], show);
        OutputDebugStringW(dbg);
        ShowWindow(arr[i], show ? SW_SHOW : SW_HIDE);
    }
}

static void on_button_click(HWND hwnd, int id, HWND ctr)
{
    wchar_t log[128];
    wchar_t btnName[128];
    GetWindowTextW(ctr, btnName, _countof(btnName));
    wsprintfW(log, L"%s가 눌렸습니다.\n", btnName);
    OutputDebugStringW(log);

    switch (id) {
    case 1001: // 홈
        toggleWindow(mainHWNDs, countMainCtrs, TRUE);
        toggleWindow(regHWNDs, countRegCtrs, FALSE);
        toggleWindow(searchHWNDs, countSearchCtrs, FALSE);
        break;

    case 1002: // 등록
        toggleWindow(mainHWNDs, countMainCtrs, FALSE);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);

        toggleWindow(regHWNDs, countRegCtrs, TRUE);
        for (int i = 0; i < countRegEdits; i++) {
            setPlaceholder(&regStructs[i]);
        }
        break;

    case 1003: { // 조회
        toggleWindow(mainHWNDs, countMainCtrs, FALSE);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        toggleWindow(searchHWNDs, countSearchCtrs, TRUE);

        wchar_t* lists = get_all_lists(); // malloc된 버퍼 반환 가정
        show_lists(lists);
        break;
    }

    case 1004: // 퀴즈(아직 미구현)
        toggleWindow(mainHWNDs, countMainCtrs, FALSE);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        toggleWindow(searchHWNDs, 1, TRUE);
        break;

    case 1009: // 단어 등록 버튼
        add_word(hwnd);
        break;
    }
}

static void on_edit_setfocus(HWND hwndCtl)
{
    for (int i = 0; i < countRegEdits; i++) {
        if (hwndCtl == regStructs[i].hwnd) {
            wchar_t buf[128];
            GetWindowTextW(hwndCtl, buf, _countof(buf));

            wchar_t expected[128];
            wsprintfW(expected, L"%sを入力してください", regStructs[i].name);

            if (wcscmp(buf, expected) == 0) {
                SetWindowTextW(hwndCtl, L"");
            }
        }
    }
}

static void on_edit_killfocus(HWND hwndCtl)
{
    for (int i = 0; i < countRegEdits; i++) {
        if (hwndCtl == regStructs[i].hwnd) {
            wchar_t buf[128];
            GetWindowTextW(hwndCtl, buf, _countof(buf));

            if (buf[0] == L'\0') {
                setPlaceholder(&regStructs[i]);
            }
        }
    }
}

static void show_lists(wchar_t* lists)
{
    if (!lists) return;

    wchar_t* context = NULL;
    wchar_t* line = wcstok_s(lists, L"\n", &context);

    while (line != NULL) {
        if (!listview_contains(hListViewType, line)) {
            LVITEMW item = { 0 };
            item.mask = LVIF_TEXT;
            item.iItem = ListView_GetItemCount(hListViewType);
            item.iSubItem = 0;
            item.pszText = line;

            ListView_InsertItem(hListViewType, &item);
        }
        line = wcstok_s(NULL, L"\n", &context);
    }

    free(lists); // get_all_lists에서 malloc 했다면 여기서 해제
}

static void show_words(void)
{
    const Word* words = dict_get_all();
    size_t count = dict_count();

    ListView_DeleteAllItems(hListViewWord);

    for (int i = 0; i < (int)count; i++) {

        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.iSubItem = 0;
        item.pszText = words[i].kanji;

        ListView_InsertItem(hListViewWord, &item);

        ListView_SetItemText(hListViewWord, i, 1, words[i].kana);
        ListView_SetItemText(hListViewWord, i, 2, words[i].meaning);
        ListView_SetItemText(hListViewWord, i, 3, words[i].example);
    }
}

static void add_word(HWND hwnd)
{
    Word w = { 0 };
    readText(hEditKanji, w.kanji, _countof(w.kanji));
    readText(hEditKana, w.kana, _countof(w.kana));
    readText(hEditMeaning, w.meaning, _countof(w.meaning));
    readText(hEditExample, w.example, _countof(w.example));
    w.type = 0;
    w.proficiency = 0;

    int rc = dict_add(&w);
    if (rc == 0) {
        int n = (int)dict_count();
        wchar_t ok[160];
        wsprintfW(ok, L"登録しました。（現在 %d 件）", n);
        MessageBoxW(hwnd, ok, L"OK", MB_OK | MB_ICONINFORMATION);

        // 디버그용 메시지박스는 나중에 필요 없으면 지우셔도 됩니다.
        wchar_t dbg[256];
        wsprintfW(dbg, L"[登録OK] count=%d, last=%s / %s\n", n, w.kanji, w.kana);
        OutputDebugStringW(dbg);

        dict_save();
    }
}

static BOOL listview_contains(HWND hList, const wchar_t* text)
{
    int count = ListView_GetItemCount(hList);

    wchar_t buf[256];
    for (int i = 0; i < count; i++) {
        ListView_GetItemText(hList, i, 0, buf, 256);
        if (wcscmp(buf, text) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}
