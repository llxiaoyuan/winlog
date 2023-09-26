#pragma once
#include <stdint.h>

#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif
	char* my_sprintf_alloc(const char* Format, ...);
	wchar_t* my_wsprintf_alloc(const wchar_t* Format, ...);
#ifdef __cplusplus
}
#endif


