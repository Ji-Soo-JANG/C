// ui.h
#pragma once
#include <windows.h>

// main 쪽에서 쓰는 것만 extern
extern HWND debug_panel;
LRESULT CALLBACK EditWndProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);

// WndProc에서 호출할 UI 엔트리 포인트
void ui_on_create(HWND hwnd, LPCREATESTRUCT pcs);
void ui_on_command(HWND hwnd, int id, int code, HWND hwnd_ctl);
BOOL ui_on_notify(HWND hwnd, WPARAM w_param, LPARAM l_param, LRESULT* result);
void ui_on_destroy(void);

void ui_word_on_create(HWND hwnd, LPCREATESTRUCT pcs);
void ui_word_edit_on_command(HWND hwnd, int id, int code, HWND hwnd_ctl);

void create_quiz_page(HWND hwnd_parent, HINSTANCE h_inst);
void quiz_show(int correct_idx);
void is_correct(void);