#pragma once
#include "dictionary.h"

extern const wchar_t* save_path;

int save_all_csv(wchar_t* path);
Word* read_csv(wchar_t* path, size_t* out_count);
int create_csv(wchar_t* path);
wchar_t* get_all_list_names(void);