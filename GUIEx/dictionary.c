#include "dictionary.h"
#include "fileio.h"

static Word g_words[WORD_MAX];
static int g_count = 0;

int dict_init(void) {
	g_count = 0;
	return 0;
}

int dict_save(const Word* w) {
	if (!w) return -1;
	return save_all_csv();
}

void dict_shutdown(void) {
	// csv 파일 등록
}

int dict_add(const Word* w) {
	if (!w) return -1;
	if (g_count >= WORD_MAX) return -2;
	
	g_words[g_count++] = *w;


	return 0;
}

int dict_count(void) {
	return g_count;
}

const Word* dict_get(int index) {
	if (index < 0 || index >= g_count) return NULL;
	return &g_words[index];
}
