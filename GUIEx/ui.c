#include "ui.h"
#include "dictionary.h"
#include <CommCtrl.h>
#include <stdio.h>
#pragma comment(lib, "Comctl32.lib")

// ===== 전역 UI 핸들 =====
HWND debug_panel;
HFONT font;

HWND btn_home;
HWND btn_add_words, btn_edit_word, btn_delete_word, btn_show_quiz;
HWND btn_search, lv_list, lv_word;
HWND edit_kanji, edit_kana, edit_meaning, edit_example, btn_add, btn_save, btn_cancle, cmb_list;
HWND quiz_question, quiz_option_btn[4], quiz_result, quiz_next_btn;

static HWND search_hwnds[6];
static HWND edit_hwnds[8];
static HWND quiz_hwnds[7];
static wchar_t g_current_list_name[MAX_PATH] = L"";
static int correct_pos, chosen_pos;
static int option_idxs[4];
static int quiz_current = 0;


#define COUNT_SEARCH_CTRS (sizeof(search_hwnds)/ sizeof(search_hwnds[0]))
#define COUNT_EDIT_CTRS (sizeof(edit_hwnds)/ sizeof(edit_hwnds[0]))
#define COUNT_QUIZ_CTRS (sizeof(quiz_hwnds)/ sizeof(quiz_hwnds[0]))

static HINSTANCE g_hInst = NULL;

// ===== 내부 헬퍼 함수 선언 =====
static BOOL read_text(HWND h, wchar_t* out, int cap);
static void toggle_window(HWND arr[], int count, BOOL show);
static void on_button_click(HWND hwnd, int id, HWND ctr);
static void show_lists(wchar_t* lists);
static void show_words(void);
static void hide_words(void);
static void add_word(wchar_t* list);
static BOOL listview_contains(HWND list, const wchar_t* text);

// ====== 공용 엔트리 포인트 ======

void ui_on_create(HWND hwnd, LPCREATESTRUCT pcs)
{
    g_hInst = pcs->hInstance;

    // 폰트 생성
    font = CreateFontW(
        18, 0, 0, 0,
        FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        VARIABLE_PITCH, L"MS UI Gothic"
    );

    // 홈 버튼
    btn_home = CreateWindowW(
        L"Button", L"ホームへ",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        30, 500, 100, 50,
        hwnd, (HMENU)1001, pcs->hInstance, NULL
    );
    SendMessageW(btn_home, WM_SETFONT, (WPARAM)font, TRUE);

    // 조회 화면 컨트롤
    lv_list = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
        30, 50, 100, 450, hwnd, (HMENU)3001, pcs->hInstance, NULL);

    LVCOLUMNW col_type = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    col_type.cx = 100;
    col_type.pszText = L"リスト";
    ListView_InsertColumn(lv_list, 0, &col_type);

    lv_word = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS,
        130, 50, 600, 450, hwnd, (HMENU)3002, pcs->hInstance, NULL);

    LVCOLUMNW col_word = { LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM };
    col_word.cx = 150; col_word.pszText = L"漢字";  ListView_InsertColumn(lv_word, 0, &col_word);
    col_word.cx = 150; col_word.pszText = L"カナ";  ListView_InsertColumn(lv_word, 1, &col_word);
    col_word.cx = 150; col_word.pszText = L"意味";  ListView_InsertColumn(lv_word, 2, &col_word);
    col_word.cx = 150; col_word.pszText = L"例文";  ListView_InsertColumn(lv_word, 3, &col_word);

    DWORD ex_style = ListView_GetExtendedListViewStyle(lv_word);
    ex_style |= LVS_EX_FULLROWSELECT;          
    ListView_SetExtendedListViewStyle(lv_word, ex_style);

    btn_add_words = CreateWindowW(L"Button", L"追加", WS_CHILD | WS_VISIBLE | WS_BORDER,
        330, 500, 100, 50, hwnd, (HMENU)1002, pcs->hInstance, NULL);
    btn_edit_word = CreateWindowW(L"Button", L"修正", WS_CHILD | WS_VISIBLE | WS_BORDER,
        430, 500, 100, 50, hwnd, (HMENU)1003, pcs->hInstance, NULL);
    btn_delete_word = CreateWindowW(L"Button", L"削除", WS_CHILD | WS_VISIBLE | WS_BORDER,
        530, 500, 100, 50, hwnd, (HMENU)1004, pcs->hInstance, NULL);
    btn_show_quiz = CreateWindowW(L"Button", L"クイズ", WS_CHILD | WS_VISIBLE | WS_BORDER,
        630, 500, 100, 50, hwnd, (HMENU)1005, pcs->hInstance, NULL);

    SendMessageW(btn_add_words, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(btn_edit_word, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(btn_delete_word, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(btn_show_quiz, WM_SETFONT, (WPARAM)font, TRUE);
    
    search_hwnds[0] = lv_list;
    search_hwnds[1] = lv_word;
    search_hwnds[2] = btn_add_words;
    search_hwnds[3] = btn_edit_word;
    search_hwnds[4] = btn_delete_word;
    search_hwnds[5] = btn_show_quiz;

    toggle_window(search_hwnds, COUNT_SEARCH_CTRS, TRUE);
    wchar_t* lists = get_all_lists();
    show_lists(lists);

}

void ui_on_command(HWND hwnd, int id, int code, HWND hwnd_ctl)
{
    if (code == BN_CLICKED) {
        on_button_click(hwnd, id, hwnd_ctl);
    }
    else if (code == EN_SETFOCUS) {
        //on_edit_setfocus(hwndCtl);
    }
    else if (code == EN_KILLFOCUS) {
        //on_edit_killfocus(hwndCtl);
    }
}

BOOL ui_on_notify(HWND hwnd, WPARAM w_param, LPARAM l_param, LRESULT* result)
{
    LPNMHDR nmhdr = (LPNMHDR)l_param;

    // 리스트 이름 리스트뷰에서 온 통지인지
    if (nmhdr->hwndFrom == lv_list) {
        NMLISTVIEW* nmlv = (NMLISTVIEW*)l_param;
        int index = nmlv->iItem;
        int count = ListView_GetItemCount(lv_list);
        int plus_idx = count - 1;

        if (nmhdr->code == LVN_ITEMCHANGED) { 
            if ((nmlv->uChanged & LVIF_STATE) &&
                (nmlv->uNewState & LVIS_SELECTED)) {

                if (index >= 0 && index != plus_idx) {
                    wchar_t buf[256];
                    ListView_GetItemText(lv_list, index, 0, buf, 256);
                    wcsncpy_s(g_current_list_name, _countof(g_current_list_name), buf, _TRUNCATE);
                    /*wchar_t deb[256];
                    wsprintfW(deb, L"list : %ls\n", g_currentListName);
                    OutputDebugStringW(deb);*/
                    set_dict(g_current_list_name);
                    show_words();
                }
            }
            *result = 0;
            return TRUE;

        }

        if (nmhdr->code == LVN_ITEMACTIVATE) {
            if (index == plus_idx) {
                ListView_EditLabel(lv_list, index);
            }
            *result = 0;
            return TRUE;
        }

        if (nmhdr->code == LVN_ENDLABELEDIT) {
            NMLVDISPINFOW* info = (NMLVDISPINFOW*)l_param;
            const wchar_t* name = info->item.pszText;

            if (info->item.pszText == NULL || info->item.pszText[0] == L'\0') {
                *result = FALSE;
                return TRUE;
            }

            new_dict(name);

            ListView_DeleteAllItems(lv_list);
            wchar_t* lists = get_all_lists();
            show_lists(lists);

            //OutputDebugStringW(name);

            *result = FALSE;
            return TRUE;
        }

    }

    return FALSE;
}

void ui_on_destroy(void)
{
    dict_save();

    if (font) {
        DeleteObject(font);
        font = NULL;
    }
}

// ===== 내부 헬퍼 구현부 =====

static BOOL read_text(HWND h, wchar_t* out, int cap)
{
    wchar_t tmp[512];
    GetWindowTextW(h, tmp, _countof(tmp));
    wcsncpy_s(out, cap, tmp, _TRUNCATE);
    return TRUE;
}

static void toggle_window(HWND arr[], int count, BOOL show)
{
    wchar_t dbg[64];
    for (int i = 0; i < count; i++) {
        wsprintfW(dbg, L"toggleWindow: HWND=0x%p, show=%d\n", arr[i], show);
        OutputDebugStringW(dbg);
        ShowWindow(arr[i], show ? SW_SHOW : SW_HIDE);
    }
}

static void on_button_click(HWND hwnd, int id, HWND ctr){
    wchar_t log[128];
    wchar_t btn_name[128];
    GetWindowTextW(ctr, btn_name, _countof(btn_name));
    wsprintfW(log, L"%s가 눌렸습니다.\n", btn_name);
    OutputDebugStringW(log);

    switch (id) {
    case 1001: // 홈
        hide_words();
        toggle_window(search_hwnds, COUNT_SEARCH_CTRS, TRUE);
        toggle_window(quiz_hwnds, COUNT_QUIZ_CTRS, FALSE);
        SetFocus(hwnd);

        break;
    
    case 1002: { // 추가 버튼
        int word_idx = -1;

        HWND edit_wnd = CreateWindowExW(
            WS_EX_CONTROLPARENT,
            L"EditWindowClass",
            L"追加",
            WS_OVERLAPPEDWINDOW,
            100, 100, 400, 300,
            hwnd,
            NULL,
            g_hInst,
            (LPVOID)word_idx
        );

        if (edit_wnd) {
            ShowWindow(edit_wnd, SW_SHOW);
            UpdateWindow(edit_wnd);
        }
        break;
    }

    case 1003:{// 수정 버튼
        /*
        1. 선택된 listView 받아오기(index)
        2. 새로운 창 생성 - hwnd(Edit - 한자/카나/뜻/예문, Button - 수정/취소)
        3. g_words에서 해당 index의 값 변경
        */

        int word_idx = ListView_GetNextItem(lv_word, -1, LVNI_SELECTED);
        if (word_idx == -1) {
            break;
        }

        HWND edit_wnd = CreateWindowExW(
            WS_EX_CONTROLPARENT,
            L"EditWindowClass",
            L"修正",
            WS_OVERLAPPEDWINDOW,
            100, 100, 400, 300,
            hwnd,
            NULL,
            g_hInst,
            (LPVOID)word_idx
        );

        if (edit_wnd) {
            ShowWindow(edit_wnd, SW_SHOW);
            UpdateWindow(edit_wnd);
        }

        break;
    } 

    case 1004: { // 삭제 버튼
        /*
        1. 선택된 listView 받아오기(index)
        2. listView의 단어를 g_words에서 지우기
        3. listView를 제거 - 포커스도 다시 세팅
        4. g_count 다시 세기
        */

        int word_idx = ListView_GetNextItem(lv_word, -1, LVNI_SELECTED);
        if (word_idx == -1) {
            break;
        }

        int result = MessageBox(hwnd, L"削除しますか", L"削除確認", MB_OKCANCEL);
        if (result == IDOK) {
            dict_delete(word_idx);
            show_words();
        }
        
        break;
    }

    case 1005: { // 퀴즈
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        toggle_window(search_hwnds, COUNT_SEARCH_CTRS, FALSE);

        quiz_current = 0;
        if (dict_count() < 4) {
            MessageBoxW(hwnd, L"少なくても４つ以上の単語を登録してください。", L"最小単語不足", MB_OK);
            hide_words();
            toggle_window(search_hwnds, COUNT_SEARCH_CTRS, TRUE);
            toggle_window(quiz_hwnds, COUNT_QUIZ_CTRS, FALSE);
            SetFocus(hwnd);

            break;
        }
    
        create_quiz_page(hwnd, g_hInst);
        toggle_window(quiz_hwnds, COUNT_QUIZ_CTRS, TRUE);
        dict_mix();
        quiz_show(quiz_current);

        break;
    }

    case 5100: {
        chosen_pos = option_idxs[0];
        is_correct();
        break;
    }

    case 5101: {
        chosen_pos = option_idxs[1];
        is_correct();
        break;
    }

    case 5102: {
        chosen_pos = option_idxs[2];
        is_correct();
        break;
    }

    case 5103: {
        chosen_pos = option_idxs[3];
        is_correct();
        break;
    }

    case 5200: {
        quiz_show(++quiz_current);
        SetWindowTextW(quiz_result, L"");
        break;
    }

    }
}

static void show_lists(wchar_t* lists){
    ListView_DeleteAllItems(lv_list);

    wchar_t* context = NULL;
    wchar_t* line = wcstok_s(lists, L"\n", &context);

    while (line != NULL) {
        if (!listview_contains(lv_list, line)) {
            LVITEMW item = { 0 };
            item.mask = LVIF_TEXT;
            item.iItem = ListView_GetItemCount(lv_list);
            item.iSubItem = 0;
            item.pszText = line;

            ListView_InsertItem(lv_list, &item);
        }
        line = wcstok_s(NULL, L"\n", &context);
    }

    free(lists); 

    int count = ListView_GetItemCount(lv_list);
    if (count > 0) {
        wchar_t buf[256];
        ListView_GetItemText(lv_list, count - 1, 0, buf, 256);

        if (wcscmp(buf, L"+ 新しいリスト") == 0) {
            return;
        }
    }

    LVITEMW item = { 0 };
    item.mask = LVIF_TEXT;
    item.iItem = ListView_GetItemCount(lv_list);
    item.iSubItem = 0;
    item.pszText = L"+ 新しいリスト";
    ListView_InsertItem(lv_list, &item);
}

static void show_words(void)
{
    const Word* words = dict_get_all();
    size_t count = dict_count();

    ListView_DeleteAllItems(lv_word);

    for (int i = 0; i < (int)count; i++) {

        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.iSubItem = 0;
        item.pszText = words[i].kanji;

        ListView_InsertItem(lv_word, &item);

        ListView_SetItemText(lv_word, i, 1, words[i].kana);
        ListView_SetItemText(lv_word, i, 2, words[i].meaning);
        ListView_SetItemText(lv_word, i, 3, words[i].example);
    }
}

static void hide_words(void) {
    if (IsWindowVisible(lv_list)) {
        ListView_SetItemState(lv_list, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    }
    if (lv_word) {
        ListView_DeleteAllItems(lv_word);
    }
}

static void add_word(wchar_t* list)
{
    dict_save();
    set_dict(list);

    Word new_words = { 0 };
    read_text(edit_kanji, new_words.kanji, _countof(new_words.kanji));
    read_text(edit_kana, new_words.kana, _countof(new_words.kana));
    read_text(edit_meaning, new_words.meaning, _countof(new_words.meaning));
    read_text(edit_example, new_words.example, _countof(new_words.example));
    new_words.type = 0;
    new_words.proficiency = 0;

    int add_result = dict_add(&new_words);
    if (add_result == 0) {
        int current_count = (int)dict_count();
        dict_save();
    }
}

static BOOL listview_contains(HWND list, const wchar_t* text)
{
    int count = ListView_GetItemCount(list);

    wchar_t buf[256];
    for (int i = 0; i < count; i++) {
        ListView_GetItemText(list, i, 0, buf, 256);
        if (wcscmp(buf, text) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// 수정창 부
void ui_word_on_create(HWND hwnd, LPCREATESTRUCT pcs) {
    int word_idx = (int)(INT_PTR)pcs->lpCreateParams;

    SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)word_idx);

    BOOL is_add_mode = (word_idx < 0);
    BOOL is_revise_mode = (word_idx >= 0);

    // 베이스 레이아웃 설정
    const int dlg_width = 420;
    const int margin_x = 20;
    const int margin_y = 20;
    const int label_width = 60;
    const int edit_width = 260;
    const int height = 28;
    const int gap_y = 34;

    int x_label = margin_x;
    int x_edit = margin_x + label_width + 10;
    int y = margin_y;

    CreateWindowW(
        L"STATIC", L"リスト:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        x_label, y + 6, label_width, height,
        hwnd, NULL, pcs->hInstance, NULL
    );

    cmb_list = CreateWindowW(
        L"COMBOBOX", NULL,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST,
        x_edit, y, edit_width, 200,
        hwnd, (HMENU)3010, pcs->hInstance, NULL
    );

    // 콤보 내용 채우기
    wchar_t* lists = get_all_lists();
    if (lists) {
        wchar_t* context = NULL;
        wchar_t* line = wcstok_s(lists, L"\n", &context);

        while (line) {
            if (line[0] != L'\0') {
                SendMessageW(cmb_list, CB_ADDSTRING, 0, (LPARAM)line);
            }
            line = wcstok_s(NULL, L"\n", &context);
        }
        free(lists);
    }
    SendMessageW(cmb_list, CB_SETCURSEL, 0, 0);



    y += gap_y + 10;

    // 漢字
    CreateWindowW(
        L"STATIC", L"漢字:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        x_label, y + 6, label_width, height,
        hwnd, NULL, pcs->hInstance, NULL
    );
    edit_kanji = CreateWindowW(
        L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP ,
        x_edit, y, edit_width, height,
        hwnd, (HMENU)3011, pcs->hInstance, NULL
    );

    y += gap_y;

    // カナ
    CreateWindowW(
        L"STATIC", L"カナ:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        x_label, y + 6, label_width, height,
        hwnd, NULL, pcs->hInstance, NULL
    );
    edit_kana = CreateWindowW(
        L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP ,
        x_edit, y, edit_width, height,
        hwnd, (HMENU)3012, pcs->hInstance, NULL
    );

    y += gap_y;

    // 意味
    CreateWindowW(
        L"STATIC", L"意味:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        x_label, y + 6, label_width, height,
        hwnd, NULL, pcs->hInstance, NULL
    );
    edit_meaning = CreateWindowW(
        L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP ,
        x_edit, y, edit_width, height,
        hwnd, (HMENU)3013, pcs->hInstance, NULL
    );

    y += gap_y;

    // 例文
    CreateWindowW(
        L"STATIC", L"例文:",
        WS_CHILD | WS_VISIBLE | SS_RIGHT,
        x_label, y + 6, label_width, height,
        hwnd, NULL, pcs->hInstance, NULL
    );
    edit_example = CreateWindowW(
        L"Edit", NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        x_edit, y, edit_width, height,
        hwnd, (HMENU)3014, pcs->hInstance, NULL
    );

    const int btn_width = 90;
    const int btn_height = 30;
    const int btn_gap_x = 20;
    const int bottom_margin = 20;

    int btnY = y + gap_y + 20;
    RECT rc;
    GetClientRect(hwnd, &rc);
    int center_x = (rc.right - rc.left) / 2;

    // 두 버튼이므로:  [Cancel] [Add/Save]
    int btn_ok_x = center_x - (btn_width + btn_gap_x / 2);
    int btn_cancel_x = center_x + (btn_gap_x / 2);

    btn_add = CreateWindowW(
        L"Button", L"追加",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        btn_ok_x, btnY, btn_width, btn_height,
        hwnd, (HMENU)3015, pcs->hInstance, NULL
    );
    btn_save = CreateWindowW(
        L"Button", L"保存",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        btn_ok_x, btnY, btn_width, btn_height,
        hwnd, (HMENU)3016, pcs->hInstance, NULL
    );
    btn_cancle = CreateWindowW(
        L"Button", L"キャンセル",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        btn_cancel_x, btnY, btn_width, btn_height,
        hwnd, (HMENU)3017, pcs->hInstance, NULL
    );

    int idx = (int)SendMessageW(cmb_list, CB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)g_current_list_name);
    if (idx != CB_ERR) {
        SendMessageW(cmb_list, CB_SETCURSEL, idx, 0);
    }

    if (is_add_mode) {
        ShowWindow(btn_add, SW_SHOW);
        ShowWindow(btn_save, SW_HIDE);

        SetWindowTextW(edit_kanji, L"");
        SetWindowTextW(edit_kana, L"");
        SetWindowTextW(edit_meaning, L"");
        SetWindowTextW(edit_example, L"");
    }
    else {
        ShowWindow(btn_add, SW_HIDE);
        ShowWindow(btn_save, SW_SHOW);

        // g_currentListName 과 일치하는 항목을 콤보에서 찾기

        EnableWindow(cmb_list, FALSE);


        const Word* w = dict_get(word_idx);
        SetWindowTextW(edit_kanji, w->kanji);
        SetWindowTextW(edit_kana, w->kana);
        SetWindowTextW(edit_meaning, w->meaning);
        SetWindowTextW(edit_example, w->example);
    }

    SendMessageW(edit_kanji, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(edit_kana, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(edit_meaning, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(edit_example, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(btn_add, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(btn_save, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(btn_cancle, WM_SETFONT, (WPARAM)font, TRUE);

    edit_hwnds[0] = edit_kanji;
    edit_hwnds[1] = edit_kana;
    edit_hwnds[2] = edit_meaning;
    edit_hwnds[3] = edit_example;
    edit_hwnds[4] = btn_add;
    edit_hwnds[5] = btn_save;
    edit_hwnds[6] = btn_cancle;
}

void ui_word_edit_on_command(HWND hwnd, int id, int code, HWND hwndCtl) {
    int word_idx = (int)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    /*wchar_t buf[32];
    wsprintfW(buf, L"%d", wordIdx);*/

    Word new_word = { 0 };
    read_text(edit_kanji, new_word.kanji, _countof(new_word.kanji));
    read_text(edit_kana, new_word.kana, _countof(new_word.kana));
    read_text(edit_meaning, new_word.meaning, _countof(new_word.meaning));
    read_text(edit_example, new_word.example, _countof(new_word.example));
    new_word.type = 0;
    new_word.proficiency = 0;

    switch (id) {
    case 3015: { // 추가
        int idx = (int)SendMessageW(cmb_list, CB_GETCURSEL, 0, 0);
        int len = (int)SendMessage(cmb_list, CB_GETLBTEXTLEN, idx, 0);
        wchar_t* list = malloc((len + 1) * sizeof(wchar_t));
        SendMessage(cmb_list, CB_GETLBTEXT, idx, (LPARAM)list);

        add_word(list);
        show_words();
        free(list);
        DestroyWindow(hwnd);
        break;
    }
    case 3016: // 수정
        if (dict_revise(word_idx, &new_word) == 0) {
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
    if (!quiz_question) {
        quiz_question = CreateWindowExW(
            0, L"STATIC",
            L"ここに問題が表示されます。",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            130, 50, 600, 40,
            hwndParent, (HMENU)4001, hInst, NULL
        );

        int x1 = 150, x2 = 450;
        int y1 = 120, y2 = 180;
        int w = 250, h = 40;

        quiz_option_btn[0] = CreateWindowExW(
            0, L"BUTTON", L"選択肢1",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x1, y1, w, h,
            hwndParent, (HMENU)5100, hInst, NULL
        );
        quiz_option_btn[1] = CreateWindowExW(
            0, L"BUTTON", L"選択肢2",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x2, y1, w, h,
            hwndParent, (HMENU)5101, hInst, NULL
        );
        quiz_option_btn[2] = CreateWindowExW(
            0, L"BUTTON", L"選択肢3",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x1, y2, w, h,
            hwndParent, (HMENU)5102, hInst, NULL
        );
        quiz_option_btn[3] = CreateWindowExW(
            0, L"BUTTON", L"選択肢4",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x2, y2, w, h,
            hwndParent, (HMENU)5103, hInst, NULL
        );

        // 결과 표시
        quiz_result = CreateWindowExW(
            0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            130, 240, 600, 30,
            hwndParent, (HMENU)4002, hInst, NULL
        );

        // 다음 문제 버튼
        quiz_next_btn = CreateWindowExW(
            0, L"BUTTON", L"次の問題",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            350, 280, 150, 35,
            hwndParent, (HMENU)5200, hInst, NULL
        );

        quiz_hwnds[0] = quiz_question;
        quiz_hwnds[1] = quiz_option_btn[0];
        quiz_hwnds[2] = quiz_option_btn[1];
        quiz_hwnds[3] = quiz_option_btn[2];
        quiz_hwnds[4] = quiz_option_btn[3];
        quiz_hwnds[5] = quiz_result;
        quiz_hwnds[6] = quiz_next_btn;

        for (int i = 0; i < 7; i++) {
            SendMessageW(quiz_hwnds[i], WM_SETFONT, (WPARAM)font, TRUE);
        }
    }
}

void quiz_show(int corret_idx) {
    int count = dict_count();
    
    if (corret_idx >= count) {
        return;
    }

    correct_pos = corret_idx;

    const Word* words = dict_get_all();
    SetWindowTextW(quiz_question, words[corret_idx].kanji);

    option_idxs[3] = corret_idx;

    for (int i = 0; i < 3; i++) {
        while (1) {
            int r = rand() % count;
            if (r == corret_idx) continue;

            BOOL dup = FALSE;
            for (int j = 0; j < i; j++) {
                if (option_idxs[j] == r) {
                    dup = TRUE;
                    break;
                }
            }
            if (dup) continue;

            option_idxs[i] = r;
            break;
        }
    }

    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = option_idxs[i];
        option_idxs[i] = option_idxs[j];
        option_idxs[j] = t;
    }

    for (int i = 0; i < 4; i++) {
        SetWindowTextW(quiz_option_btn[i], dict_get(option_idxs[i])->kana);
    }
}

void is_correct() {
    if (chosen_pos == correct_pos) {
        SetWindowTextW(quiz_result, L"正解です！");
        plus_proficiency(correct_pos);
    }
    else {
        SetWindowTextW(quiz_result, L"残念…");
        minus_proficiency(correct_pos);
    }
}
