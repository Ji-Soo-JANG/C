#include "ui.h"
#include "dictionary.h"
#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

// ===== 전역 UI 핸들 =====
HWND hDebugPanel;
HFONT hFont;

HWND hBtnHome;
HWND hBtnRegisterPage, hBtnSearchPage, hBtnQuizPage;
HWND hBtnEditWord, hBtnDelteWord;
HWND hEditKanji, hEditKana, hEditMeaning, hEditExample, hBtnRegister;
HWND hBtnSearch, hListViewType, hListViewWord;
HWND hWndEditKanji, hWndEditKana, hWndEditMeaning, hWndEditExample, hWndBtnSave, hWndBtnCancle;
HWND hQuizQuestion, hQuizOptionBtn[4], hQuizResult, hQuizNextBtn;

static HWND mainHWNDs[3];
static struct registerCtrs regStructs[4];
static HWND regHWNDs[5];
static HWND searchHWNDs[4];
static HWND editHWNDs[6];
static HWND quizHWNDs[7];
static int correctPos, chosenPos;
static int optionIdxs[4];
static int quiz_current = 0;

#define countMainCtrs   (sizeof(mainHWNDs)  / sizeof(mainHWNDs[0]))
#define countRegEdits   (sizeof(regStructs) / sizeof(regStructs[0]))
#define countRegCtrs    (sizeof(regHWNDs)   / sizeof(regHWNDs[0]))
#define countSearchCtrs (sizeof(searchHWNDs)/ sizeof(searchHWNDs[0]))
#define countEditCtrs (sizeof(editHWNDs)/ sizeof(editHWNDs[0]))
#define countQuizCtrs (sizeof(quizHWNDs)/ sizeof(quizHWNDs[0]))

static HINSTANCE g_hInst = NULL;

// ===== 내부 헬퍼 함수 선언 =====
static BOOL readText(HWND h, wchar_t* out, int cap);
static void setPlaceholder(struct registerCtrs* com);
static void toggleWindow(HWND arr[], int count, BOOL show);
static void on_button_click(HWND hwnd, int id, HWND ctr);
static void on_edit_setfocus(HWND hwndCtl);
static void on_edit_killfocus(HWND hwndCtl);
static void show_lists(wchar_t* lists);
static void show_words(void);
static void hide_words(void);
static void add_word(HWND hwnd);
static BOOL listview_contains(HWND hList, const wchar_t* text);

// ====== 공용 엔트리 포인트 ======

void UI_OnCreate(HWND hwnd, LPCREATESTRUCT pcs)
{
    g_hInst = pcs->hInstance;

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
    hEditKanji = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        50, 50, 200, 50, hwnd, (HMENU)2001, pcs->hInstance, NULL);
    hEditKana = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        50, 100, 200, 50, hwnd, (HMENU)2002, pcs->hInstance, NULL);
    hEditMeaning = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        50, 150, 200, 50, hwnd, (HMENU)2003, pcs->hInstance, NULL);
    hEditExample = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        50, 200, 200, 50, hwnd, (HMENU)2004, pcs->hInstance, NULL);
    hBtnRegister = CreateWindowW(L"Button", L"単語登録", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        50, 250, 200, 50, hwnd, (HMENU)2005, pcs->hInstance, NULL);

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
        WS_CHILD | WS_VISIBLE | LVS_REPORT,
        30, 50, 100, 450, hwnd, (HMENU)3001, pcs->hInstance, NULL);

    LVCOLUMNW colType = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    colType.cx = 100;
    colType.pszText = L"リスト";
    ListView_InsertColumn(hListViewType, 0, &colType);

    hListViewWord = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS,
        130, 50, 600, 450, hwnd, (HMENU)3002, pcs->hInstance, NULL);

    LVCOLUMNW colWord = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    colWord.cx = 150; colWord.pszText = L"漢字";  ListView_InsertColumn(hListViewWord, 0, &colWord);
    colWord.cx = 150; colWord.pszText = L"カナ";  ListView_InsertColumn(hListViewWord, 1, &colWord);
    colWord.cx = 150; colWord.pszText = L"意味";  ListView_InsertColumn(hListViewWord, 2, &colWord);
    colWord.cx = 150; colWord.pszText = L"例文";  ListView_InsertColumn(hListViewWord, 3, &colWord);

    DWORD exStyle = ListView_GetExtendedListViewStyle(hListViewWord);
    exStyle |= LVS_EX_FULLROWSELECT;          
    ListView_SetExtendedListViewStyle(hListViewWord, exStyle);

    hBtnEditWord = CreateWindowW(L"Button", L"修正", WS_CHILD | WS_VISIBLE | WS_BORDER,
        530, 500, 100, 50, hwnd, (HMENU)3003, pcs->hInstance, NULL);
    hBtnDelteWord = CreateWindowW(L"Button", L"削除", WS_CHILD | WS_VISIBLE | WS_BORDER,
        630, 500, 100, 50, hwnd, (HMENU)3004, pcs->hInstance, NULL);

    SendMessageW(hBtnEditWord, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hBtnDelteWord, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    searchHWNDs[0] = hListViewType;
    searchHWNDs[1] = hListViewWord;
    searchHWNDs[2] = hBtnEditWord;
    searchHWNDs[3] = hBtnDelteWord;

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
                (pNMLV->uNewState & LVIS_SELECTED)) {

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
        hide_words();
        toggleWindow(mainHWNDs, countMainCtrs, TRUE);
        toggleWindow(regHWNDs, countRegCtrs, FALSE);
        toggleWindow(searchHWNDs, countSearchCtrs, FALSE);
        toggleWindow(quizHWNDs, countQuizCtrs, FALSE);
        SetFocus(hwnd);

        break;

    case 1002: // 등록
        toggleWindow(mainHWNDs, countMainCtrs, FALSE);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);

        toggleWindow(regHWNDs, countRegCtrs, TRUE);
        for (int i = 0; i < countRegEdits; i++) {
            setPlaceholder(&regStructs[i]);
        }
        
        SetFocus(hEditKanji);
        break;

    case 1003: { // 조회
        toggleWindow(mainHWNDs, countMainCtrs, FALSE);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        toggleWindow(searchHWNDs, countSearchCtrs, TRUE);

        wchar_t* lists = get_all_lists(); 
        show_lists(lists);
        break;
    }

    case 1004: // 퀴즈(아직 미구현)
        toggleWindow(mainHWNDs, countMainCtrs, FALSE);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        toggleWindow(searchHWNDs, 1, FALSE);

        quiz_current = 0;
        if (dict_count() < 4) {
            MessageBoxW(hwnd, L"少なくても４つ以上の単語を登録してください。", L"最小単語不足", MB_OK);
            hide_words();
            toggleWindow(mainHWNDs, countMainCtrs, TRUE);
            toggleWindow(regHWNDs, countRegCtrs, FALSE);
            toggleWindow(searchHWNDs, countSearchCtrs, FALSE);
            toggleWindow(quizHWNDs, countQuizCtrs, FALSE);
            SetFocus(hwnd);

            break;
        }

        create_quiz_page(hwnd, g_hInst);
        toggleWindow(quizHWNDs, countQuizCtrs, TRUE);
        dict_mix();
        quiz_show(quiz_current);

        break;

    case 2005: // 단어 등록 버튼
        add_word(hwnd);
        break;

    case 3003:{// 수정 버튼
        /*
        1. 선택된 listView 받아오기(index)
        2. 새로운 창 생성 - hwnd(Edit - 한자/카나/뜻/예문, Button - 수정/취소)
        3. g_words에서 해당 index의 값 변경
        */

        int wordIdx = ListView_GetNextItem(hListViewWord, -1, LVNI_SELECTED);
        if (wordIdx == -1) {
            break;
        }

        HWND hEditWnd = CreateWindowExW(
            0,
            L"EditWindowClass",
            L"修正",
            WS_OVERLAPPEDWINDOW,
            100, 100, 400, 300,
            hwnd,
            NULL,
            g_hInst,
            (LPVOID)wordIdx
        );

        if (hEditWnd) {
            ShowWindow(hEditWnd, SW_SHOW);
            UpdateWindow(hEditWnd);
        }

        break;
    } 
    case 3004: { // 삭제 버튼
        /*
        1. 선택된 listView 받아오기(index)
        2. listView의 단어를 g_words에서 지우기
        3. listView를 제거 - 포커스도 다시 세팅
        4. g_count 다시 세기
        */

        int wordIdx = ListView_GetNextItem(hListViewWord, -1, LVNI_SELECTED);
        if (wordIdx == -1) {
            break;
        }

        int result = MessageBox(hwnd, L"削除しますか", L"削除確認", MB_OKCANCEL);
        if (result == IDOK) {
            dict_delete(wordIdx);
            show_words();
        }
        
        break;
    }
    case 4100:
        chosenPos = optionIdxs[0];
        is_correct();
        break;

    case 4101:
        chosenPos = optionIdxs[1];
        is_correct();
        break;

    case 4102:
        chosenPos = optionIdxs[2];
        is_correct();
        break;

    case 4103:
        chosenPos = optionIdxs[3];
        is_correct();
        break;

    case 4200:
        quiz_show(++quiz_current);
        SetWindowTextW(hQuizResult, L"");
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

    free(lists); 
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

static void hide_words(void) {
    if (IsWindowVisible(hListViewType)) {
        ListView_SetItemState(hListViewType, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    }
    if (hListViewWord) {
        ListView_DeleteAllItems(hListViewWord);
    }
}

static void add_word(HWND hwnd)
{
    Word new_words = { 0 };
    readText(hEditKanji, new_words.kanji, _countof(new_words.kanji));
    readText(hEditKana, new_words.kana, _countof(new_words.kana));
    readText(hEditMeaning, new_words.meaning, _countof(new_words.meaning));
    readText(hEditExample, new_words.example, _countof(new_words.example));
    new_words.type = 0;
    new_words.proficiency = 0;

    int add_result = dict_add(&new_words);
    if (add_result == 0) {
        int current_count = (int)dict_count();
        wchar_t msg_buffer[160];
        wsprintfW(msg_buffer, L"登録しました。（現在 %d 件）", current_count);
        MessageBoxW(hwnd, msg_buffer, L"OK", MB_OK | MB_ICONINFORMATION);

        //wchar_t dbg[256];
        //wsprintfW(dbg, L"[登録OK] count=%d, last=%s / %s\n", current_count, new_words.kanji, new_words.kana);
        //OutputDebugStringW(dbg);

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


// 수정창 부
void UI_WordEdit_OnCreate(HWND hwnd, LPCREATESTRUCT pcs) {
    int wordIdx = (int)(INT_PTR)pcs->lpCreateParams;

    // window에 저장
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)wordIdx);

    const Word* w = dict_get(wordIdx);

    // Edit창용 TextBox들 생성
    int xLabel = 20;
    int xEdit = 120;
    int y = 30;
    int height = 28;
    int gap = 45;

    // 라벨들 생성
    CreateWindowW(L"STATIC", L"漢字:", WS_CHILD | WS_VISIBLE,
        xLabel, y, 80, height, hwnd, NULL, pcs->hInstance, NULL);

    hWndEditKanji = CreateWindowW(L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        xEdit, y, 150, height,
        hwnd, (HMENU)3011, pcs->hInstance, NULL);

    y += gap;

    CreateWindowW(L"STATIC", L"カナ:", WS_CHILD | WS_VISIBLE,
        xLabel, y, 80, height, hwnd, NULL, pcs->hInstance, NULL);

    hWndEditKana = CreateWindowW(L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        xEdit, y, 150, height,
        hwnd, (HMENU)3012, pcs->hInstance, NULL);

    y += gap;

    CreateWindowW(L"STATIC", L"意味:", WS_CHILD | WS_VISIBLE,
        xLabel, y, 80, height, hwnd, NULL, pcs->hInstance, NULL);

    hWndEditMeaning = CreateWindowW(L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        xEdit, y, 150, height,
        hwnd, (HMENU)3013, pcs->hInstance, NULL);

    y += gap;

    CreateWindowW(L"STATIC", L"例文:", WS_CHILD | WS_VISIBLE,
        xLabel, y, 80, height, hwnd, NULL, pcs->hInstance, NULL);

    hWndEditExample = CreateWindowW(L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        xEdit, y, 150, height,
        hwnd, (HMENU)3014, pcs->hInstance, NULL);

    hWndBtnSave = CreateWindowW(L"Button", L"貯蔵", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        290, 30, 70, 50, hwnd, (HMENU)3015, pcs->hInstance, NULL);
    hWndBtnCancle = CreateWindowW(L"Button", L"キャンセル", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        290, 100, 70, 50, hwnd, (HMENU)3016, pcs->hInstance, NULL);

    SendMessageW(hWndEditKanji, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndEditKana, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndEditMeaning, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndEditExample, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndBtnSave, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndBtnCancle, WM_SETFONT, (WPARAM)hFont, TRUE);

    editHWNDs[0] = hWndEditKanji;
    editHWNDs[1] = hWndEditKana;
    editHWNDs[2] = hWndEditMeaning;
    editHWNDs[3] = hWndEditExample;
    editHWNDs[4] = hWndBtnSave;
    editHWNDs[5] = hWndBtnCancle;

    //// 값 채우기
    SetWindowTextW(hWndEditKanji, w->kanji);
    SetWindowTextW(hWndEditKana, w->kana);
    SetWindowTextW(hWndEditMeaning, w->meaning);
    SetWindowTextW(hWndEditExample, w->example);
}
void UI_WordEdit_OnCommand(HWND hwnd, int id, int code, HWND hwndCtl) {
    int wordIdx = (int)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    /*wchar_t buf[32];
    wsprintfW(buf, L"%d", wordIdx);*/

    Word newWord = { 0 };
    readText(hWndEditKanji, newWord.kanji, _countof(newWord.kanji));
    readText(hWndEditKana, newWord.kana, _countof(newWord.kana));
    readText(hWndEditMeaning, newWord.meaning, _countof(newWord.meaning));
    readText(hWndEditExample, newWord.example, _countof(newWord.example));
    newWord.type = 0;
    newWord.proficiency = 0;

    switch (id) {
    case 3015:
        // 수정한거 저장하는 코드
        //MessageBoxW(hwnd, "", buf, MB_OK);
        if (dict_revise(wordIdx, &newWord) == 0) {
            dict_save();
            show_words();     // 메인창 리스트 갱신
        }
        DestroyWindow(hwnd);
        break;
    case 3016:
        DestroyWindow(hwnd);
        return 0;
        // eidt창 없애기

    }
}

void create_quiz_page(HWND hwndParent, HINSTANCE hInst)
{
    // 질문 영역
    if (!hQuizQuestion) {
        hQuizQuestion = CreateWindowExW(
            0, L"STATIC",
            L"ここに問題が表示されます。",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            130, 50, 600, 40,
            hwndParent, (HMENU)4001, hInst, NULL
        );

        int x1 = 150, x2 = 450;
        int y1 = 120, y2 = 180;
        int w = 250, h = 40;

        hQuizOptionBtn[0] = CreateWindowExW(
            0, L"BUTTON", L"選択肢1",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x1, y1, w, h,
            hwndParent, (HMENU)4100, hInst, NULL
        );
        hQuizOptionBtn[1] = CreateWindowExW(
            0, L"BUTTON", L"選択肢2",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x2, y1, w, h,
            hwndParent, (HMENU)4101, hInst, NULL
        );
        hQuizOptionBtn[2] = CreateWindowExW(
            0, L"BUTTON", L"選択肢3",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x1, y2, w, h,
            hwndParent, (HMENU)4102, hInst, NULL
        );
        hQuizOptionBtn[3] = CreateWindowExW(
            0, L"BUTTON", L"選択肢4",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x2, y2, w, h,
            hwndParent, (HMENU)4103, hInst, NULL
        );

        // 결과 표시
        hQuizResult = CreateWindowExW(
            0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            130, 240, 600, 30,
            hwndParent, (HMENU)4002, hInst, NULL
        );

        // 다음 문제 버튼
        hQuizNextBtn = CreateWindowExW(
            0, L"BUTTON", L"次の問題",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            350, 280, 150, 35,
            hwndParent, (HMENU)4200, hInst, NULL
        );

        quizHWNDs[0] = hQuizQuestion;
        quizHWNDs[1] = hQuizOptionBtn[0];
        quizHWNDs[2] = hQuizOptionBtn[1];
        quizHWNDs[3] = hQuizOptionBtn[2];
        quizHWNDs[4] = hQuizOptionBtn[3];
        quizHWNDs[5] = hQuizResult;
        quizHWNDs[6] = hQuizNextBtn;

        for (int i = 0; i < 7; i++) {
            SendMessageW(quizHWNDs[i], WM_SETFONT, (WPARAM)hFont, TRUE);
        }
    }
}

void quiz_show(int correctIdx) {
    int count = dict_count();
    
    if (correctIdx >= count) {
        return;
    }

    correctPos = correctIdx;

    const Word* words = dict_get_all();
    SetWindowTextW(hQuizQuestion, words[correctIdx].kanji);

    optionIdxs[3] = correctIdx;

    for (int i = 0; i < 3; i++) {
        while (1) {
            int r = rand() % count;
            if (r == correctIdx) continue;

            BOOL dup = FALSE;
            for (int j = 0; j < i; j++) {
                if (optionIdxs[j] == r) {
                    dup = TRUE;
                    break;
                }
            }
            if (dup) continue;

            optionIdxs[i] = r;
            break;
        }
    }

    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = optionIdxs[i];
        optionIdxs[i] = optionIdxs[j];
        optionIdxs[j] = t;
    }

    for (int i = 0; i < 4; i++) {
        SetWindowTextW(hQuizOptionBtn[i], dict_get(optionIdxs[i])->kana);
    }
}
void is_correct() {
    if (chosenPos == correctPos) {
        SetWindowTextW(hQuizResult, L"正解です！");
        plus_proficiency(correctPos);
    }
    else {
        SetWindowTextW(hQuizResult, L"残念…");
        minus_proficiency(correctPos);
    }
}