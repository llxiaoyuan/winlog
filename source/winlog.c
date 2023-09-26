#include "winlog.h"
#include "my_sprint.h"
#include "my_sprint_alloc.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

static size_t string_length_a(const char* str)
{
	const char* s;
	for (s = str; *s; ++s)
		;
	return (s - str);
}

static size_t string_length_w(const wchar_t* str)
{
	const wchar_t* s;
	for (s = str; *s; ++s)
		;
	return (s - str);
}

static void* my_memcpy(void* _Dst, void const* _Src, size_t _Size)
{
	char* dp = (char*)_Dst;
	char* sp = (char*)_Src;
	for (size_t i = 0; i < _Size; i++) {
		dp[i] = sp[i];
	}
	return _Dst;
}

static void* my_memset(void* _Dst, int _Val, size_t _Size)
{
	char* q = (char*)_Dst;
	while (_Size--) {
		*q++ = _Val;
	}
	return _Dst;
}

static void* Alloc(SIZE_T BufferSize)
{
	//return VirtualAlloc(NULL, BufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	//VOID* BaseAddress = NULL;
	//SIZE_T RegionSize = BufferSize;
	//ctx->_NtAllocateVirtualMemory((HANDLE)-1, &BaseAddress, 0, &RegionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	//return BaseAddress;

	return HeapAlloc(GetProcessHeap(), 0, BufferSize);
}

static void Free(void* Buffer)
{
	//VirtualFree(Buffer, 0, MEM_RELEASE);

	//VOID* BaseAddress = Buffer;
	//SIZE_T RegionSize = 0;
	//ctx->_NtFreeVirtualMemory((HANDLE)-1, &BaseAddress, &RegionSize, MEM_RELEASE);

	HeapFree(GetProcessHeap(), 0, Buffer);
}

static int to_hex_char(int value, int letter_base)
{
	int c;
	if (value >= 10) {
		c = value - 10 + letter_base;
	}
	else {
		c = value + '0';
	}
	return c;
}

static char* BufferToStringA(unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line)
{
	size_t str_len;
	if (show_in_c_array) {
		str_len = (size * 4) + (size - 1) + ((size + count_per_line - 1) / count_per_line);
	}
	else {
		str_len = (size * 2) + (size - 1) + ((size + count_per_line - 1) / count_per_line);
	}

	char* buffer_alloc = (char*)Alloc((str_len + 1) * sizeof(char));
	if (buffer_alloc != NULL) {
		char* str = buffer_alloc;
		size_t i;
		for (i = 0; i < size; i++) {
			if (show_in_c_array) {
				my_sprint_c(&str, '0');
				my_sprint_c(&str, 'x');
				my_sprint_c(&str, to_hex_char(buffer[i] >> 4, 'A'));
				my_sprint_c(&str, to_hex_char(buffer[i] & 0x0F, 'A'));
				if (i != size - 1) {
					my_sprint_c(&str, ',');
				}
			}
			else {
				my_sprint_c(&str, to_hex_char(buffer[i] >> 4, 'A'));
				my_sprint_c(&str, to_hex_char(buffer[i] & 0x0F, 'A'));
				if (i != size - 1) {
					my_sprint_c(&str, ' ');
				}
			}
			if (((i + 1) % count_per_line) == 0) {
				my_sprint_c(&str, '\n');
			}
		}
		if ((i % count_per_line) != 0) {
			my_sprint_c(&str, '\n');
		}
		*str = '\0';
	}
	return buffer_alloc;
}

static wchar_t* BufferToStringW(unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line)
{
	size_t str_len;
	if (show_in_c_array) {
		str_len = (size * 4) + (size - 1) + ((size + count_per_line - 1) / count_per_line);
	}
	else {
		str_len = (size * 2) + (size - 1) + ((size + count_per_line - 1) / count_per_line);
	}

	wchar_t* buffer_alloc = (wchar_t*)Alloc((str_len + 1) * sizeof(wchar_t));
	if (buffer_alloc != NULL) {
		wchar_t* str = buffer_alloc;
		size_t i;
		for (i = 0; i < size; i++) {
			if (show_in_c_array) {
				my_wsprint_c(&str, '0');
				my_wsprint_c(&str, 'x');
				my_wsprint_c(&str, to_hex_char(buffer[i] >> 4, 'A'));
				my_wsprint_c(&str, to_hex_char(buffer[i] & 0x0F, 'A'));
				if (i != size - 1) {
					my_wsprint_c(&str, ',');
				}
			}
			else {
				my_wsprint_c(&str, to_hex_char(buffer[i] >> 4, 'A'));
				my_wsprint_c(&str, to_hex_char(buffer[i] & 0x0F, 'A'));
				if (i != size - 1) {
					my_wsprint_c(&str, ' ');
				}
			}
			if (((i + 1) % count_per_line) == 0) {
				my_wsprint_c(&str, '\n');
			}
		}
		if ((i % count_per_line) != 0) {
			my_wsprint_c(&str, '\n');
		}
		*str = '\0';
	}
	return buffer_alloc;
}

typedef struct LogCtx
{
	wchar_t* _FileName;
} LogCtx;

static void Log(LogCtx* ctx, const void* Buffer, size_t BufferSize)
{
	HANDLE hFile = CreateFileW(ctx->_FileName, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, Buffer, (DWORD)BufferSize, NULL, NULL);
		CloseHandle(hFile);
	}
}

LogCtx* __cdecl LogOpen(const wchar_t* FileName)
{
	LogCtx* ctx = (LogCtx*)Alloc(sizeof(LogCtx));
	if (ctx != NULL) {
		size_t FileNameLengthInByte = (string_length_w(FileName) + 1) * sizeof(*FileName);
		ctx->_FileName = (wchar_t*)Alloc(FileNameLengthInByte);
		if (ctx->_FileName != NULL) {
			my_memcpy(ctx->_FileName, FileName, FileNameLengthInByte);
		}
	}
	return ctx;
}

void __cdecl LogPopup(LogCtx* ctx)
{
	//0x5c003f003f005c == '\\\0?\0?\0\\' == "\\??\\"
	wchar_t ApplicationName[MAX_PATH] = L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
	wchar_t* CommandLine = my_wsprintf_alloc(L"%s Get-Content \"%s\" -Wait -Tail 30", ApplicationName, (*(uint64_t*)ctx->_FileName == 0x5c003f003f005c) ? ctx->_FileName + 4 : ctx->_FileName);
	if (CommandLine) {
		STARTUPINFOW StartupInfo = { 0 };
		StartupInfo.cb = sizeof(StartupInfo);
		PROCESS_INFORMATION ProcessInformation = { 0 };
		BOOL success = CreateProcessW(ApplicationName, CommandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &StartupInfo, &ProcessInformation);
		if (success) {
			//WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
			CloseHandle(ProcessInformation.hProcess);
			CloseHandle(ProcessInformation.hThread);
		}
		Free(CommandLine);
	}
}

void __cdecl LogClose(LogCtx* ctx)
{
	if (ctx != NULL) {
		if (ctx->_FileName != NULL) {
			Free(ctx->_FileName);
		}
		Free(ctx);
	}
}

void __cdecl LogStringA(LogCtx* ctx, const char* String)
{
	if (ctx != NULL && ctx->_FileName != NULL) {
		Log(ctx, String, string_length_a(String) * sizeof(*String));
	}
}

void __cdecl LogStringW(LogCtx* ctx, const wchar_t* String)
{
	if (ctx != NULL && ctx->_FileName != NULL) {
		Log(ctx, String, string_length_w(String) * sizeof(*String));
	}
}

void __cdecl LogFormatA(LogCtx* ctx, const char* Format, ...)
{
	if (ctx != NULL && ctx->_FileName != NULL) {
		va_list ArgList;
		va_start(ArgList, Format);
		size_t BufferCount = my_sprint(NULL, Format, ArgList);//no null end
		size_t BufferSize = BufferCount * sizeof(char);
		char* Buffer = (char*)Alloc(BufferSize);
		if (Buffer != NULL) {
			my_sprint(Buffer, Format, ArgList);
			Log(ctx, Buffer, BufferSize);
			Free(Buffer);
		}
		va_end(ArgList);
	}
}

void __cdecl LogFormatW(LogCtx* ctx, const wchar_t* Format, ...)
{
	if (ctx != NULL && ctx->_FileName != NULL) {
		va_list ArgList;
		va_start(ArgList, Format);
		size_t BufferCount = my_wsprint(NULL, Format, ArgList);//no null end
		size_t BufferSize = BufferCount * sizeof(wchar_t);
		wchar_t* Buffer = (wchar_t*)Alloc(BufferSize);
		if (Buffer != NULL) {
			my_wsprint(Buffer, Format, ArgList);
			Log(ctx, Buffer, BufferSize);
			Free(Buffer);
		}
		va_end(ArgList);
	}
}

void __cdecl LogFormatBufferA(LogCtx* ctx, unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line)
{
	if (ctx != NULL && ctx->_FileName != NULL) {
		char* str = BufferToStringA(buffer, size, show_in_c_array, count_per_line);
		if (str != NULL) {
			LogStringA(ctx, str);
			Free(str);
		}
	}
}

void __cdecl LogFormatBufferW(LogCtx* ctx, unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line)
{
	if (ctx != NULL && ctx->_FileName != NULL) {
		wchar_t* str = BufferToStringW(buffer, size, show_in_c_array, count_per_line);
		if (str != NULL) {
			LogStringW(ctx, str);
			Free(str);
		}
	}
}

void __cdecl DebugStringA(const char* String)
{
	OutputDebugStringA(String);
}

void __cdecl DebugStringW(const wchar_t* String)
{
	OutputDebugStringW(String);
}

void __cdecl DebugFormatA(const char* Format, ...)
{
	va_list ArgList;
	va_start(ArgList, Format);
	size_t BufferCount = my_sprint(NULL, Format, ArgList);//no null end
	char* Buffer = (char*)Alloc((BufferCount + 1) * sizeof(char));
	if (Buffer != NULL) {
		my_sprint(Buffer, Format, ArgList);
		Buffer[BufferCount] = '\0';
		DebugStringA(Buffer);
		Free(Buffer);
	}
	va_end(ArgList);
}

void __cdecl DebugFormatW(const wchar_t* Format, ...)
{
	va_list ArgList;
	va_start(ArgList, Format);
	size_t BufferCount = my_wsprint(NULL, Format, ArgList);//no null end
	wchar_t* Buffer = (wchar_t*)Alloc((BufferCount + 1) * sizeof(wchar_t));
	if (Buffer != NULL) {
		my_wsprint(Buffer, Format, ArgList);
		Buffer[BufferCount] = '\0';
		DebugStringW(Buffer);
		Free(Buffer);
	}
	va_end(ArgList);
}

void __cdecl DebugFormatBufferA(unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line)
{
	char* str = BufferToStringA(buffer, size, show_in_c_array, count_per_line);
	if (str != NULL) {
		DebugStringA(str);
		Free(str);
	}
}

void __cdecl DebugFormatBufferW(unsigned char* buffer, size_t size, int show_in_c_array, size_t count_per_line)
{
	wchar_t* str = BufferToStringW(buffer, size, show_in_c_array, count_per_line);
	if (str != NULL) {
		DebugStringW(str);
		Free(str);
	}
}
