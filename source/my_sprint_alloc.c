#include "my_sprint_alloc.h"
#include "my_sprint.h"
#include "AllocFree.h"
#include <stdarg.h>

char* my_sprintf_alloc(const char* Format, ...)
{
	va_list ArgList;
	va_start(ArgList, Format);
	size_t BufferCount = my_sprint(NULL, Format, ArgList);//no null end
	char* Buffer = (char*)Alloc((BufferCount + 1) * sizeof(char));
	if (Buffer != NULL) {
		my_sprint(Buffer, Format, ArgList);
		Buffer[BufferCount] = '\0';
	}
	va_end(ArgList);
	return Buffer;
}

wchar_t* my_wsprintf_alloc(const wchar_t* Format, ...)
{
	va_list ArgList;
	va_start(ArgList, Format);
	size_t BufferCount = my_wsprint(NULL, Format, ArgList);//no null end
	wchar_t* Buffer = (wchar_t*)Alloc((BufferCount + 1) * sizeof(wchar_t));
	if (Buffer != NULL) {
		my_wsprint(Buffer, Format, ArgList);
		Buffer[BufferCount] = '\0';
	}
	va_end(ArgList);
	return Buffer;
}