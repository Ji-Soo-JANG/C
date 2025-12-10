#include "fileio.h"
#include <stdio.h>

const wchar_t* directory = L".";
const wchar_t* save_path = L"words.csv";

int save_all_csv(void) {
	FILE* fp;
	if (_wfopen_s(&fp, save_path, L"w, ccs=UTF-8") != 0) {
		return -1;
	}

	for (int i = 0; i < dict_count(); i++) {
		const Word* w = dict_get(i);
		fwprintf(fp, L"%ls,%ls,%ls,%ls,%d,%d\n", w->kanji, w->kana, w->meaning, w->example, w->proficiency, w->type);
	}

	fclose(fp);
	return 0;
};
int append_csv(const Word* w) {
	return 0;
};

int create_csv(void) {
	FILE* fp;
	if (_wfopen_s(&fp, save_path, L"r, ccs=UTF-8") != 0) {
		if (_wfopen_s(&fp, save_path, L"w, ccs=UTF-8") == 0) {
			fclose(fp);
			return 1;
		}
		else {
			return 0;
		}
	}
	fclose(fp);

	return 1;
}


/*
	1. 파일 읽고, count 구함
	2. malloc으로 words 동적 할당
	3. 다시 파일을 읽으면서 words에 word 구조체를 채워넣음
	4. words의 주소를 반환
*/
Word* read_csv(size_t* out_count) {
	FILE* fp;
	if (_wfopen_s(&fp, save_path, L"r, ccs=UTF-8") != 0 || fp == NULL) {
		if (out_count) *out_count = 0;
		return NULL;
	};
	
	wchar_t line[256];
	size_t count = 0;
	
	// count를 구함
	while (fgetws(line, 256, fp) != NULL) {
		if (line[0] != L'\n' && line[0] != L'\r' && line[0] != L'\0')
			count++;
	}
	
	// fp를 맨 처음으로 돌림
	fseek(fp, 0, SEEK_SET);

	// count가 0일 경우 처리
	if (count == 0) {
		fclose(fp);
		if (out_count) *out_count = 0;
		return NULL;
	}
	// words 동적 할당
	Word* words = malloc(sizeof(Word) * count);
	if (!words) {
		fclose(fp);
		if (out_count) *out_count = 0;
		return NULL;
	}

	// word에 값 저장, words에 저장
	size_t idx = 0;
	while (fgetws(line, _countof(line), fp) != NULL){
		line[wcscspn(line, L"\r\n")] = L'\0';
		if (line[0] == L'\0') {
			continue;  
		}

		wchar_t* ctx = NULL;
		wchar_t* token = wcstok_s(line, L",", &ctx);
		Word word = { 0 };
		if (token) {
			wcsncpy_s(word.kanji, _countof(word.kanji), token, _TRUNCATE);
			token = wcstok_s(NULL, L",", &ctx);
		}
		if (token) {
			wcsncpy_s(word.kana, _countof(word.kana), token, _TRUNCATE);
			token = wcstok_s(NULL, L",", &ctx);
		}
		if (token) {
			wcsncpy_s(word.meaning, _countof(word.meaning), token, _TRUNCATE);
			token = wcstok_s(NULL, L",", &ctx);
		}
		if (token) {
			wcsncpy_s(word.example, _countof(word.example), token, _TRUNCATE);
			token = wcstok_s(NULL, L",", &ctx);
		}
		if (token) {
			word.proficiency = _wtoi(token);
			token = wcstok_s(NULL, L",", &ctx);
		}
		if (token) {
			word.type = _wtoi(token);
			token = wcstok_s(NULL, L",", &ctx);
		}
		

		words[idx] = word;
		idx++;
			
		
	}

	// 호출부 value에 idx 저장
	*out_count = idx;
	// fp닫기
	fclose(fp);

	return words;

	
};

wchar_t* get_all_list_names() {
	wchar_t pattern[MAX_PATH];
	wsprintfW(pattern, L"%s\\*.csv", directory);

	WIN32_FIND_DATAW fd;
	HANDLE hFind = FindFirstFileW(pattern, &fd);
	if (hFind == INVALID_HANDLE_VALUE)
		return NULL;

	// 2) 임시 버퍼 
	wchar_t temp[4096] = L"";
	
	do {
		// 폴더는 제외
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			wcscat_s(temp, _countof(temp), fd.cFileName);
			wcscat_s(temp, _countof(temp), L"\n");
		}
	} while (FindNextFileW(hFind, &fd));
	
	FindClose(hFind);

	// 3) 필요한 크기만큼 malloc 해서 복사
	size_t len = wcslen(temp) + 1;
	wchar_t* result = malloc(sizeof(wchar_t) * len);
	if (!result) return NULL;

	wcscpy_s(result, len, temp);
	return result;
}
