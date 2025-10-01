#include "fileio.h"
#include <stdio.h>

const wchar_t* save_path = L"words.csv";

int save_all_csv(void) {
	FILE* fp;
	if (_wfopen_s(&fp, save_path, L"w, ccs=UTF-8") != 0) {
		return -1;
	}

	for (int i = 0; i < dict_count(); i++) {
		const Word* w = dict_get(i);
		fwprintf(fp, L"%s,%s,%s,%s,%d,%d\n", w->kanji, w->kana, w->meaning, w->example, w->proficiency, w->type);
	}

	fclose(fp);
	return 0;
};
int append_csv(const Word* w) {
	return 0;
};
int read_csv(void) {
	return 0;
};
