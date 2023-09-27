#pragma once
#include <stdint.h>

struct LogCtx;
typedef struct LogCtx LogCtx;

#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif
	LogCtx* __cdecl LogOpen(const wchar_t* FileName);

	void __cdecl LogClear(LogCtx* ctx);
	void __cdecl LogPopup(LogCtx* ctx);

	void __cdecl LogClose(LogCtx* ctx);
	void __cdecl LogStringA(LogCtx* ctx, const char* String);
	void __cdecl LogStringW(LogCtx* ctx, const wchar_t* String);
	void __cdecl LogFormatA(LogCtx* ctx, const char* Format, ...);
	void __cdecl LogFormatW(LogCtx* ctx, const wchar_t* Format, ...);
	void __cdecl LogFormatBufferA(LogCtx* ctx, unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line);
	void __cdecl LogFormatBufferW(LogCtx* ctx, unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line);

	void __cdecl DebugStringA(const char* String);
	void __cdecl DebugStringW(const wchar_t* String);
	void __cdecl DebugFormatA(const char* Format, ...);
	void __cdecl DebugFormatW(const wchar_t* Format, ...);
	void __cdecl DebugFormatBufferA(unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line);
	void __cdecl DebugFormatBufferW(unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line);
#ifdef __cplusplus
}
#endif
