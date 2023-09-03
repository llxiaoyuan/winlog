#include "winlog.h"
#include <stdint.h>

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

#define PRINT_BUFFER_LENGTH (64 + 1)

static long long get_args_value(int z_count, int h_count, int l_count, va_list args)
{
	long long value;
	if (z_count == 1) {
		value = (long long)((void*)va_arg(args, void*));
	}
	else if (h_count == 1) {
		value = (short)va_arg(args, short);
	}
	else if (h_count == 2) {
		value = (char)va_arg(args, char);
	}
	else if (l_count == 1) {
		value = (long)va_arg(args, long);
	}
	else if (l_count == 2) {
		value = (long long)va_arg(args, long long);
	}
	else {
		value = (int)va_arg(args, int);
	}
	return value;
}

static void my_print_c(char** out, int c)
{
	if (*out) {
		**out = (char)c;
		++(*out);
	}
	//else {
	//	os_putchar(c);
	//}
}

static size_t my_print_s_a(char** out, const char* s, size_t width)
{
	size_t length = string_length_a(s);

	if (length < width)
		width -= length;
	else
		width = 0;

	size_t result = 0;

	result += width;
	while (width--)
	{
		my_print_c(out, '0');
	}

	result += length;
	while (length--)
	{
		my_print_c(out, *(s++));
	}

	return result;
}

static size_t my_print_s_w(char** out, const wchar_t* s, size_t width)
{
	size_t length = string_length_w(s);

	if (length < width)
		width -= length;
	else
		width = 0;

	size_t result = 0;

	result += width;
	while (width--)
	{
		my_print_c(out, '0');
	}

	result += length;
	while (length--)
	{
		my_print_c(out, *(s++));
	}

	return result;
}

static size_t my_print_i(char** out, long long i, int radix, int is_signed, int letter_base, size_t width)
{
	//1111111111111111111111111111111111111111111111111111111111111111
	char print_buffer[PRINT_BUFFER_LENGTH];
	if (i == 0) {
		print_buffer[0] = '0';
		print_buffer[1] = '\0';
		return my_print_s_a(out, print_buffer, width);
	}
	unsigned long long u = i;
	int is_negative = 0;
	if (is_signed && radix == 10 && i < 0) {
		is_negative = 1;
		u = -i;
	}

	char* s = print_buffer;
	s += PRINT_BUFFER_LENGTH - 1;
	*s = '\0';

	while (u) {
		int t = u % radix;
		char c;
		if (t >= 10) {
			c = t - 10 + letter_base;
		}
		else {
			c = t + '0';
		}
		*(--s) = c;
		u /= radix;
	}

	if (is_negative) {
		*(--s) = '-';
	}

	return my_print_s_a(out, s, width);
}

static size_t my_print(char* out, const char* format, va_list args)
{
	size_t result = 0;

	int z_count;

	int h_count;

	int l_count;

	int pad_zero;

	size_t width;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			if (*format == '\0') {
				break;
			}
			if (*format == '%') {
				my_print_c(&out, *format);
				result++;
				continue;
			}
			if (*format == 'c') {
				my_print_c(&out, (char)va_arg(args, int));
				result++;
				continue;
			}

			if (*format == 'p') {
				result += my_print_i(&out, (long long)((void*)va_arg(args, void*)), 16, 0, 'a', sizeof(void*) * 2);
				continue;
			}

			if (*format == 'P') {
				result += my_print_i(&out, (long long)((void*)va_arg(args, void*)), 16, 0, 'A', sizeof(void*) * 2);
				continue;
			}

			if (*format == 's') {
				result += my_print_s_a(&out, (char*)va_arg(args, char*), 0);
				continue;
			}
			if (*format == 'S') {
				result += my_print_s_w(&out, (wchar_t*)va_arg(args, wchar_t*), 0);
				continue;
			}

			pad_zero = 0;
			if (*format == '0') {
				++format;
				pad_zero = 1;
			}

			width = 0;
			if (pad_zero) {
				for (; *format >= '0' && *format <= '9'; ++format) {
					width *= 10;
					width += *format - '0';
				}
			}

			z_count = 0;
			if (*format == 'z') {
				++format;
				++z_count;
			}

			h_count = 0;
			if (*format == 'h') {
				++format;
				++h_count;
			}
			if (*format == 'h') {
				++format;
				++h_count;
			}

			l_count = 0;
			if (*format == 'l') {
				++format;
				++l_count;
			}
			if (*format == 'l') {
				++format;
				++l_count;
			}

			int radix = 0;
			int is_signed = 0;
			int letter_base = 0;

			if (*format == 'd') {
				radix = 10;
				is_signed = 1;
			}
			else if (*format == 'u') {
				radix = 10;
			}
			else if (*format == 'x') {
				radix = 16;
				letter_base = 'a';
			}
			else if (*format == 'X') {
				radix = 16;
				letter_base = 'A';
			}
			else if (*format == 'b') {
				radix = 2;
			}

			if (radix != 0) {
				result += my_print_i(&out, get_args_value(z_count, h_count, l_count, args), radix, is_signed, letter_base, width);
				continue;
			}
		}
		else {
			my_print_c(&out, *format);
			result++;
		}
	}
	return result;
}

//without '\0'
static size_t my_sprintf(char* out, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t result = my_print(out, format, args);
	va_end(args);
	return result;
}

static void my_wprint_c(wchar_t** out, int c)
{
	if (*out) {
		**out = (wchar_t)c;
		++(*out);
	}
	//else {
	//	os_putchar(c);
	//}
}

static size_t my_wprint_s_a(wchar_t** out, const char* s, size_t width)
{
	size_t length = string_length_a(s);

	if (length < width)
		width -= length;
	else
		width = 0;

	size_t result = 0;

	result += width;
	while (width--)
	{
		my_wprint_c(out, '0');
	}

	result += length;
	while (length--)
	{
		my_wprint_c(out, *(s++));
	}

	return result;
}

static size_t my_wprint_s_w(wchar_t** out, const wchar_t* s, size_t width)
{
	size_t length = string_length_w(s);

	if (length < width)
		width -= length;
	else
		width = 0;

	size_t result = 0;

	result += width;
	while (width--)
	{
		my_wprint_c(out, '0');
	}

	result += length;
	while (length--)
	{
		my_wprint_c(out, *(s++));
	}

	return result;
}

static size_t my_wprint_i(wchar_t** out, long long i, int radix, int is_signed, int letter_base, size_t width)
{
	//1111111111111111111111111111111111111111111111111111111111111111
	char print_buffer[PRINT_BUFFER_LENGTH];
	if (i == 0) {
		print_buffer[0] = '0';
		print_buffer[1] = '\0';
		return my_wprint_s_a(out, print_buffer, width);
	}
	unsigned long long u = i;
	int is_negative = 0;
	if (is_signed && radix == 10 && i < 0) {
		is_negative = 1;
		u = -i;
	}

	char* s = print_buffer;
	s += PRINT_BUFFER_LENGTH - 1;
	*s = '\0';

	while (u) {
		int t = u % radix;
		char c;
		if (t >= 10) {
			c = t - 10 + letter_base;
		}
		else {
			c = t + '0';
		}
		*(--s) = c;
		u /= radix;
	}

	if (is_negative) {
		*(--s) = '-';
	}

	return my_wprint_s_a(out, s, width);
}

static size_t my_wprint(wchar_t* out, const wchar_t* format, va_list args)
{
	size_t result = 0;

	int z_count;

	int h_count;

	int l_count;

	int pad_zero;

	size_t width;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			if (*format == '\0') {
				break;
			}
			if (*format == '%') {
				my_wprint_c(&out, *format);
				result++;
				continue;
			}
			if (*format == 'c') {
				my_wprint_c(&out, va_arg(args, int));
				result++;
				continue;
			}

			if (*format == 'p') {
				result += my_wprint_i(&out, (long long)((void*)va_arg(args, void*)), 16, 0, 'a', sizeof(void*) * 2);
				continue;
			}

			if (*format == 'P') {
				result += my_wprint_i(&out, (long long)((void*)va_arg(args, void*)), 16, 0, 'A', sizeof(void*) * 2);
				continue;
			}

			if (*format == 's') {
				result += my_wprint_s_w(&out, (wchar_t*)va_arg(args, wchar_t*), 0);
				continue;
			}
			if (*format == 'S') {
				result += my_wprint_s_a(&out, (char*)va_arg(args, char*), 0);
				continue;
			}

			pad_zero = 0;
			if (*format == '0') {
				++format;
				pad_zero = 1;
			}

			width = 0;
			if (pad_zero) {
				for (; *format >= '0' && *format <= '9'; ++format) {
					width *= 10;
					width += *format - '0';
				}
			}

			z_count = 0;
			if (*format == 'z') {
				++format;
				++z_count;
			}

			h_count = 0;
			if (*format == 'h') {
				++format;
				++h_count;
			}
			if (*format == 'h') {
				++format;
				++h_count;
			}

			l_count = 0;
			if (*format == 'l') {
				++format;
				++l_count;
			}
			if (*format == 'l') {
				++format;
				++l_count;
			}

			int radix = 0;
			int is_signed = 0;
			int letter_base = 0;

			if (*format == 'd') {
				radix = 10;
				is_signed = 1;
			}
			else if (*format == 'u') {
				radix = 10;
			}
			else if (*format == 'x') {
				radix = 16;
				letter_base = 'a';
			}
			else if (*format == 'X') {
				radix = 16;
				letter_base = 'A';
			}
			else if (*format == 'b') {
				radix = 2;
			}

			if (radix != 0) {
				result += my_wprint_i(&out, get_args_value(z_count, h_count, l_count, args), radix, is_signed, letter_base, width);
				continue;
			}
		}
		else {
			my_wprint_c(&out, *format);
			result++;
		}
	}
	return result;
}

//without '\0'
static size_t my_wsprintf(wchar_t* out, const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	size_t result = my_wprint(out, format, args);
	va_end(args);
	return result;
}

#undef PRINT_BUFFER_LENGTH

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
				my_print_c(&str, '0');
				my_print_c(&str, 'x');
				my_print_c(&str, to_hex_char(buffer[i] >> 4, 'A'));
				my_print_c(&str, to_hex_char(buffer[i] & 0x0F, 'A'));
				if (i != size - 1) {
					my_print_c(&str, ',');
				}
			}
			else {
				my_print_c(&str, to_hex_char(buffer[i] >> 4, 'A'));
				my_print_c(&str, to_hex_char(buffer[i] & 0x0F, 'A'));
				if (i != size - 1) {
					my_print_c(&str, ' ');
				}
			}
			if (((i + 1) % count_per_line) == 0) {
				my_print_c(&str, '\n');
			}
		}
		if ((i % count_per_line) != 0) {
			my_print_c(&str, '\n');
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
				my_wprint_c(&str, '0');
				my_wprint_c(&str, 'x');
				my_wprint_c(&str, to_hex_char(buffer[i] >> 4, 'A'));
				my_wprint_c(&str, to_hex_char(buffer[i] & 0x0F, 'A'));
				if (i != size - 1) {
					my_wprint_c(&str, ',');
				}
			}
			else {
				my_wprint_c(&str, to_hex_char(buffer[i] >> 4, 'A'));
				my_wprint_c(&str, to_hex_char(buffer[i] & 0x0F, 'A'));
				if (i != size - 1) {
					my_wprint_c(&str, ' ');
				}
			}
			if (((i + 1) % count_per_line) == 0) {
				my_wprint_c(&str, '\n');
			}
		}
		if ((i % count_per_line) != 0) {
			my_wprint_c(&str, '\n');
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
		size_t BufferCount = my_print(NULL, Format, ArgList);//no null end
		size_t BufferSize = BufferCount * sizeof(char);
		char* Buffer = (char*)Alloc(BufferSize);
		if (Buffer != NULL) {
			my_print(Buffer, Format, ArgList);
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
		size_t BufferCount = my_wprint(NULL, Format, ArgList);//no null end
		size_t BufferSize = BufferCount * sizeof(wchar_t);
		wchar_t* Buffer = (wchar_t*)Alloc(BufferSize);
		if (Buffer != NULL) {
			my_wprint(Buffer, Format, ArgList);
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
	size_t BufferCount = my_print(NULL, Format, ArgList);//no null end
	char* Buffer = (char*)Alloc((BufferCount + 1) * sizeof(char));
	if (Buffer != NULL) {
		my_print(Buffer, Format, ArgList);
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
	size_t BufferCount = my_wprint(NULL, Format, ArgList);//no null end
	wchar_t* Buffer = (wchar_t*)Alloc((BufferCount + 1) * sizeof(wchar_t));
	if (Buffer != NULL) {
		my_wprint(Buffer, Format, ArgList);
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
