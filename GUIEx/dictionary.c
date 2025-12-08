#include "dictionary.h"
#include "fileio.h"

// c에서 static은 파일 내부 전용
static Word* g_words;
static size_t g_count = 0;
static size_t g_capacity = 4;

int dict_init(void) {
	int is_success = create_csv();
	if (!is_success) {
		return 0;
	}

	g_words = read_csv(&g_count);  

	if (g_count > 0) {
		g_capacity = g_count;
	}
	else {
		g_capacity = 4;  
		g_words = malloc(g_capacity * sizeof(Word));
		if (!g_words) {
			g_capacity = 0;
			return 0;
		}
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

	if (g_capacity == 0) {
		g_capacity = 4;
		g_words = malloc(g_capacity * sizeof(Word));
		if (!g_words) {
			return -1;
		}
	}

	if (g_count >= g_capacity) {
		size_t new_capacity = g_capacity * 2;
		if (new_capacity == 0) new_capacity = 4;

		Word* tmp = realloc(g_words, new_capacity * sizeof(Word));
		if (!tmp) {
			return -1;
		}

		g_words = tmp;
		g_capacity = new_capacity;
	}

	g_words[g_count] = *w;
	g_count++;

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

wchar_t* get_all_lists() {
	return get_all_list_names();
}

int dict_delete(int index) {
	if (index < 0 || index >= g_count)
		return -1;

	for (int i = index; i < g_count - 1; i++) {
		g_words[i] = g_words[i + 1];
	}

	g_count--;

	return 0;
}

int dict_revise(int index, Word* w) {
	if (index < 0 || index >= g_count)
		return -1;

	g_words[index] = *w;

	return 0;
}

void dict_mix() {
	if (g_count <= 1) return;

	static int seeded = 0;
	if (!seeded) {
		srand((unsigned int)time(NULL));
		seeded = 1;
	}

	for (size_t i = g_count - 1; i > 0; i--) {
		size_t j = (size_t)(rand() % (int)(i + 1));

		Word tmp = g_words[i];
		g_words[i] = g_words[j];
		g_words[j] = tmp;
	}
}

void plus_proficiency(int index) {
	g_words[index].proficiency++;
}

void minus_proficiency(int index) {
	g_words[index].proficiency--;
}