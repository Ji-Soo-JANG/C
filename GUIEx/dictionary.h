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

	int dict_add(const Word* w);
	size_t dict_count(void);
	int dict_save(void);
	void set_dict(wchar_t* list);
	const Word* dict_get(int index);
	const Word* dict_get_all(void);
	wchar_t* get_all_lists(void);
	int dict_delete(int index);
	int dict_revise(int index, Word* w);
	void dict_mix(void);
	void plus_proficiency(int index);
	void minus_proficiency(int index);
	void new_dict(wchar_t* name);

#ifdef __cplusplus
}
#endif

