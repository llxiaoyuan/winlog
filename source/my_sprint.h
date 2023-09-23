#pragma once
#include <stdint.h>

#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif
	void my_sprint_c(char** out, int c);
	size_t my_sprint_s_a(char** out, const char* s, size_t width);
	size_t my_sprint_s_w(char** out, const wchar_t* s, size_t width);
	size_t my_sprint_i(char** out, long long i, int radix, int is_signed, int letter_base, size_t width);
	size_t my_sprint(char* out, const char* format, va_list args);
	size_t my_sprintf(char* out, const char* format, ...);

	void my_wsprint_c(wchar_t** out, int c);
	size_t my_wsprint_s_a(wchar_t** out, const char* s, size_t width);
	size_t my_wsprint_s_w(wchar_t** out, const wchar_t* s, size_t width);
	size_t my_wsprint_i(wchar_t** out, long long i, int radix, int is_signed, int letter_base, size_t width);
	size_t my_wsprint(wchar_t* out, const wchar_t* format, va_list args);
	size_t my_wsprintf(wchar_t* out, const wchar_t* format, ...);
#ifdef __cplusplus
}
#endif


