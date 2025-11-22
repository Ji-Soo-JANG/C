#include "dictionary.h"
#include "fileio.h"

// c에서 static은 파일 내부 전용
static Word* g_words;
static size_t g_count = 0;

int dict_init(void) {
	int is_success = create_csv();
	if (is_success) {
		g_words = read_csv(&g_count);
	}
	else {
		return 0;
	}
	return 1;
}

int dict_save(void) {
	return save_all_csv();
}

void dict_shutdown(void) {
	// csv 파일 등록
}

int dict_add(const Word* w) {
	if (!w) return -1;
	g_words[g_count++] = *w;


	return 0;
}

size_t dict_count(void) {
	return g_count;
}

const Word* dict_get(int index) {
	if (index < 0 || index >= g_count) return NULL;
	return &g_words[index];
}

const Word* dict_get_all() {
	return g_words;
}