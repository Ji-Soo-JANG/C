#pragma once
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif
	//------------------------------------------------------
	// ¼³Á¤ °ª
	//------------------------------------------------------

	typedef struct {
		wchar_t kanji[128];
		wchar_t kana[128];
		wchar_t meaning[256];
		int type;
		int proficiency;
		wchar_t example[256];
	} Word;

	int dict_init(void);
	void dict_shutdown(void);
	int dict_add(const Word* w);
	size_t dict_count(void);
	int dict_save(void);
	const Word* dict_get(int index);
	const Word* dict_get_all(void);

#ifdef __cplusplus
}
#endif

