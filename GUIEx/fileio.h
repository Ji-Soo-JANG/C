#pragma once
#include "dictionary.h"

extern const wchar_t* save_path;

int save_all_csv(void);
int append_csv(const Word* w);
int read_csv(void);

