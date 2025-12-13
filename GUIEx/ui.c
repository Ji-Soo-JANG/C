#include "ui.h"
#include "dictionary.h"
#include <CommCtrl.h>
#include <stdio.h>
#pragma comment(lib, "Comctl32.lib")

// ===== 전역 UI 핸들 =====
HWND hDebugPanel;
HFONT hFont;

HWND hBtnHome;
HWND hBtnRegisterPage, hBtnSearchPage, hBtnQuizPage;
HWND hBtnAddWords,hBtnEditWord, hBtnDelteWord, hBtnShowQuiz;
HWND hEditKanji, hEditKana, hEditMeaning, hEditExample, hBtnRegister;
HWND hBtnSearch, hListViewList, hListViewWord;
HWND hWndEditKanji, hWndEditKana, hWndEditMeaning, hWndEditExample, hWndBtnAdd, hWndBtnSave, hWndBtnCancle, hWndListCombo;
HWND hQuizQuestion, hQuizOptionBtn[4], hQuizResult, hQuizNextBtn;

static HWND mainHWNDs[3];
static struct registerCtrs regStructs[4];
static HWND regHWNDs[5];
static HWND searchHWNDs[6];
static HWND editHWNDs[8];
static HWND quizHWNDs[7];
static wchar_t g_currentListName[MAX_PATH] = L"";
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
static void add_word(wchar_t* list);
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
    hEditExample = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
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
    hListViewList = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
        30, 50, 100, 450, hwnd, (HMENU)3001, pcs->hInstance, NULL);

    LVCOLUMNW colType = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    colType.cx = 100;
    colType.pszText = L"リスト";
    ListView_InsertColumn(hListViewList, 0, &colType);

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

    hBtnAddWords = CreateWindowW(L"Button", L"追加", WS_CHILD | WS_VISIBLE | WS_BORDER,
        330, 500, 100, 50, hwnd, (HMENU)3003, pcs->hInstance, NULL);
    hBtnEditWord = CreateWindowW(L"Button", L"修正", WS_CHILD | WS_VISIBLE | WS_BORDER,
        430, 500, 100, 50, hwnd, (HMENU)3004, pcs->hInstance, NULL);
    hBtnDelteWord = CreateWindowW(L"Button", L"削除", WS_CHILD | WS_VISIBLE | WS_BORDER,
        530, 500, 100, 50, hwnd, (HMENU)3005, pcs->hInstance, NULL);
    hBtnShowQuiz = CreateWindowW(L"Button", L"クイズ", WS_CHILD | WS_VISIBLE | WS_BORDER,
        630, 500, 100, 50, hwnd, (HMENU)3006, pcs->hInstance, NULL);

    SendMessageW(hBtnAddWords, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hBtnEditWord, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hBtnDelteWord, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hBtnShowQuiz, WM_SETFONT, (WPARAM)hFont, TRUE);
    
    searchHWNDs[0] = hListViewList;
    searchHWNDs[1] = hListViewWord;
    searchHWNDs[2] = hBtnAddWords;
    searchHWNDs[3] = hBtnEditWord;
    searchHWNDs[4] = hBtnDelteWord;
    searchHWNDs[5] = hBtnShowQuiz;

    toggleWindow(searchHWNDs, countSearchCtrs, FALSE);
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
    if (pNMHDR->hwndFrom == hListViewList) {
        NMLISTVIEW* pNMLV = (NMLISTVIEW*)lParam;
        int index = pNMLV->iItem;
        int count = ListView_GetItemCount(hListViewList);
        int plusIdx = count - 1;

        if (pNMHDR->code == LVN_ITEMCHANGED) { 
            if ((pNMLV->uChanged & LVIF_STATE) &&
                (pNMLV->uNewState & LVIS_SELECTED)) {

                if (index >= 0 && index != plusIdx) {
                    wchar_t buf[256];
                    ListView_GetItemText(hListViewList, index, 0, buf, 256);
                    wcsncpy_s(g_currentListName, _countof(g_currentListName), buf, _TRUNCATE);
                    /*wchar_t deb[256];
                    wsprintfW(deb, L"list : %ls\n", g_currentListName);
                    OutputDebugStringW(deb);*/
                    set_dict(g_currentListName);
                    show_words();
                }
            }
            *result = 0;
            return TRUE;

        }

        if (pNMHDR->code == LVN_ITEMACTIVATE) {
            if (index == plusIdx) {
                ListView_EditLabel(hListViewList, index);
            }
            *result = 0;
            return TRUE;
        }

        if (pNMHDR->code == LVN_ENDLABELEDIT) {
            NMLVDISPINFOW* info = (NMLVDISPINFOW*)lParam;
            HWND hList = hListViewList;
            const wchar_t* name = info->item.pszText;

            if (info->item.pszText == NULL || info->item.pszText[0] == L'\0') {
                *result = FALSE;
                return TRUE;
            }

            new_dict(name);

            wcsncpy_s(g_currentListName, _countof(g_currentListName), name, _TRUNCATE);
            set_dict(g_currentListName);
            wchar_t* lists = get_all_lists();
            show_lists(lists);

            //OutputDebugStringW(name);


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
        //add_word(hwnd);
        break;

    case 3003: { // 추가 버튼
        int wordIdx = -1;

        HWND hEditWnd = CreateWindowExW(
            WS_EX_CONTROLPARENT,
            L"EditWindowClass",
            L"追加",
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
    

    case 3004:{// 수정 버튼
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
            WS_EX_CONTROLPARENT,
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
    case 3005: { // 삭제 버튼
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
    ListView_DeleteAllItems(hListViewList);

    wchar_t* context = NULL;
    wchar_t* line = wcstok_s(lists, L"\n", &context);

    while (line != NULL) {
        if (!listview_contains(hListViewList, line)) {
            LVITEMW item = { 0 };
            item.mask = LVIF_TEXT;
            item.iItem = ListView_GetItemCount(hListViewList);
            item.iSubItem = 0;
            item.pszText = line;

            ListView_InsertItem(hListViewList, &item);
        }
        line = wcstok_s(NULL, L"\n", &context);
    }

    free(lists); 

    int count = ListView_GetItemCount(hListViewList);
    if (count > 0) {
        wchar_t buf[256];
        ListView_GetItemText(hListViewList, count - 1, 0, buf, 256);

        if (wcscmp(buf, L"+ 新しいリスト") == 0) {
            return;
        }
    }

    LVITEMW item = { 0 };
    item.mask = LVIF_TEXT;
    item.iItem = ListView_GetItemCount(hListViewList);
    item.iSubItem = 0;
    item.pszText = L"+ 新しいリスト";
    ListView_InsertItem(hListViewList, &item);
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
    if (IsWindowVisible(hListViewList)) {
        ListView_SetItemState(hListViewList, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    }
    if (hListViewWord) {
        ListView_DeleteAllItems(hListViewWord);
    }
}

static void add_word(wchar_t* list)
{
    dict_save();
    set_dict(list);

    Word new_words = { 0 };
    readText(hWndEditKanji, new_words.kanji, _countof(new_words.kanji));
    readText(hWndEditKana, new_words.kana, _countof(new_words.kana));
    readText(hWndEditMeaning, new_words.meaning, _countof(new_words.meaning));
    readText(hWndEditExample, new_words.example, _countof(new_words.example));
    new_words.type = 0;
    new_words.proficiency = 0;

    int add_result = dict_add(&new_words);
    if (add_result == 0) {
        int current_count = (int)dict_count();
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
void UI_Word_OnCreate(HWND hwnd, LPCREATESTRUCT pcs) {
    int wordIdx = (int)(INT_PTR)pcs->lpCreateParams;

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)wordIdx);

    BOOL isAddMode = (wordIdx < 0);
    BOOL isReviseMode = (wordIdx >= 0);

    // 베이스 레이아웃 설정
    const int dlgWidth = 420;
    const int marginX = 20;
    const int marginY = 20;
    const int labelWidth = 60;
    const int editWidth = 260;
    const int height = 28;
    const int gapY = 34;

    int xLabel = marginX;
    int xEdit = marginX + labelWidth + 10;
    int y = marginY;

    CreateWindowW(
        L"STATIC", L"リスト:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        xLabel, y + 6, labelWidth, height,
        hwnd, NULL, pcs->hInstance, NULL
    );

    hWndListCombo = CreateWindowW(
        L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        xEdit, y, editWidth, 200,
        hwnd, (HMENU)3010, pcs->hInstance, NULL
    );

    // 콤보 내용 채우기
    wchar_t* lists = get_all_lists();
    if (lists) {
        wchar_t* context = NULL;
        wchar_t* line = wcstok_s(lists, L"\n", &context);

        while (line) {
            if (line[0] != L'\0') {
                SendMessageW(hWndListCombo, CB_ADDSTRING, 0, (LPARAM)line);
            }
            line = wcstok_s(NULL, L"\n", &context);
        }
        free(lists);
    }
    SendMessageW(hWndListCombo, CB_SETCURSEL, 0, 0);



    y += gapY + 10;

    // 漢字
    CreateWindowW(
        L"STATIC", L"漢字:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        xLabel, y + 6, labelWidth, height,
        hwnd, NULL, pcs->hInstance, NULL
    );
    hWndEditKanji = CreateWindowW(
        L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP ,
        xEdit, y, editWidth, height,
        hwnd, (HMENU)3011, pcs->hInstance, NULL
    );

    y += gapY;

    // カナ
    CreateWindowW(
        L"STATIC", L"カナ:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        xLabel, y + 6, labelWidth, height,
        hwnd, NULL, pcs->hInstance, NULL
    );
    hWndEditKana = CreateWindowW(
        L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP ,
        xEdit, y, editWidth, height,
        hwnd, (HMENU)3012, pcs->hInstance, NULL
    );

    y += gapY;

    // 意味
    CreateWindowW(
        L"STATIC", L"意味:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        xLabel, y + 6, labelWidth, height,
        hwnd, NULL, pcs->hInstance, NULL
    );
    hWndEditMeaning = CreateWindowW(
        L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP ,
        xEdit, y, editWidth, height,
        hwnd, (HMENU)3013, pcs->hInstance, NULL
    );

    y += gapY;

    // 例文
    CreateWindowW(
        L"STATIC", L"例文:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        xLabel, y + 6, labelWidth, height,
        hwnd, NULL, pcs->hInstance, NULL
    );
    hWndEditExample = CreateWindowW(
        L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        xEdit, y, editWidth, height,
        hwnd, (HMENU)3014, pcs->hInstance, NULL
    );

    const int btnWidth = 90;
    const int btnHeight = 30;
    const int btnGapX = 20;
    const int bottomMargin = 20;

    int btnY = y + gapY + 20;
    RECT rc;
    GetClientRect(hwnd, &rc);
    int centerX = (rc.right - rc.left) / 2;

    // 두 버튼이므로:  [Cancel] [Add/Save]
    int btnXOk = centerX - (btnWidth + btnGapX / 2);
    int btnXCancel = centerX + (btnGapX / 2);

    hWndBtnAdd = CreateWindowW(
        L"Button", L"追加",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        btnXOk, btnY, btnWidth, btnHeight,
        hwnd, (HMENU)3015, pcs->hInstance, NULL
    );
    hWndBtnSave = CreateWindowW(
        L"Button", L"保存",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        btnXOk, btnY, btnWidth, btnHeight,
        hwnd, (HMENU)3016, pcs->hInstance, NULL
    );
    hWndBtnCancle = CreateWindowW(
        L"Button", L"キャンセル",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        btnXCancel, btnY, btnWidth, btnHeight,
        hwnd, (HMENU)3017, pcs->hInstance, NULL
    );

    int idx = (int)SendMessageW(hWndListCombo, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)g_currentListName);
    if (idx != CB_ERR) {
        SendMessageW(hWndListCombo, CB_SETCURSEL, idx, 0);
    }

    if (isAddMode) {
        ShowWindow(hWndBtnAdd, SW_SHOW);
        ShowWindow(hWndBtnSave, SW_HIDE);

        SetWindowTextW(hWndEditKanji, L"");
        SetWindowTextW(hWndEditKana, L"");
        SetWindowTextW(hWndEditMeaning, L"");
        SetWindowTextW(hWndEditExample, L"");
    }
    else {
        ShowWindow(hWndBtnAdd, SW_HIDE);
        ShowWindow(hWndBtnSave, SW_SHOW);

        // g_currentListName 과 일치하는 항목을 콤보에서 찾기

        EnableWindow(hWndListCombo, FALSE);


        const Word* w = dict_get(wordIdx);
        SetWindowTextW(hWndEditKanji, w->kanji);
        SetWindowTextW(hWndEditKana, w->kana);
        SetWindowTextW(hWndEditMeaning, w->meaning);
        SetWindowTextW(hWndEditExample, w->example);
    }

    SendMessageW(hWndEditKanji, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndEditKana, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndEditMeaning, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndEditExample, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndBtnAdd, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndBtnSave, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessageW(hWndBtnCancle, WM_SETFONT, (WPARAM)hFont, TRUE);

    editHWNDs[0] = hWndEditKanji;
    editHWNDs[1] = hWndEditKana;
    editHWNDs[2] = hWndEditMeaning;
    editHWNDs[3] = hWndEditExample;
    editHWNDs[4] = hWndBtnAdd;
    editHWNDs[5] = hWndBtnSave;
    editHWNDs[6] = hWndBtnCancle;
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
    case 3015: { // 추가
        int idx = (int)SendMessageW(hWndListCombo, CB_GETCURSEL, 0, 0);
        int len = (int)SendMessage(hWndListCombo, CB_GETLBTEXTLEN, idx, 0);
        wchar_t* list = malloc((len + 1) * sizeof(wchar_t));
        SendMessage(hWndListCombo, CB_GETLBTEXT, idx, (LPARAM)list);

        add_word(list);
        show_words();
        DestroyWindow(hwnd);
        break;
    }
    case 3016: // 수정
        if (dict_revise(wordIdx, &newWord) == 0) {
            dict_save();
            show_words();     
        }
        DestroyWindow(hwnd);
        break;
    case 3017: // 취소
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
