#pragma once
#include <windows.h>

extern HWND hDebugPanel;
extern HFONT hFont;         
extern HWND hBtnHome;
extern HWND hBtnRegisterPage, hBtnSearchPage, hBtnQuizPage;
extern HWND hEditKanji, hEditKana, hEditMeaning, hEditExample, hBtnRegister;
extern HWND hBtnSearch, hListViewType, hListViewWord;
extern HWND hBtnSearch, hListViewType, hListViewWord;

//// ホームのパンネル配列
extern HWND mainHWNDs[];

// パンネル構造体作成
struct registerCtrs {
    HWND hwnd;
    wchar_t name[50];
};

extern struct registerCtrs regStructs[];
extern HWND regHWNDs[]; 
extern HWND searchHWNDs[];

void createDebugPanel(HWND hwnd, LPCREATESTRUCT pcs);             
void createHFont();
void createBtnHome(HWND hwnd, LPCREATESTRUCT pcs);
void createBtnMains(HWND hwnd, LPCREATESTRUCT pcs);
void createRegCtrs(HWND hwnd, LPCREATESTRUCT pcs);
void createSearchCtrs(HWND hwnd, LPCREATESTRUCT pcs);
void clickEvent(HWND hwnd, LPARAM lParam, int code, int id);
void setFocusEvent(LPARAM lParam);
void killFocusEvent(LPARAM lParam);
void setPlaceholder(struct registerCtrs* com);
void toggleWindow(HWND arr[], int count, BOOL show);
