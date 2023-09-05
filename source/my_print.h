#pragma once
#include <stdint.h>

#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif
	void my_print_c(char** out, int c);
	size_t my_print_s_a(char** out, const char* s, size_t width);
	size_t my_print_s_w(char** out, const wchar_t* s, size_t width);
	size_t my_print_i(char** out, long long i, int radix, int is_signed, int letter_base, size_t width);
	size_t my_print(char* out, const char* format, va_list args);
	size_t my_sprintf(char* out, const char* format, ...);

	void my_wprint_c(wchar_t** out, int c);
	size_t my_wprint_s_a(wchar_t** out, const char* s, size_t width);
	size_t my_wprint_s_w(wchar_t** out, const wchar_t* s, size_t width);
	size_t my_wprint_i(wchar_t** out, long long i, int radix, int is_signed, int letter_base, size_t width);
	size_t my_wprint(wchar_t* out, const wchar_t* format, va_list args);
	size_t my_wsprintf(wchar_t* out, const wchar_t* format, ...);
#ifdef __cplusplus
}
#endif


