//#include "ntlog.h"
#include "winlog.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include "winlog.h"

int main() 
{
	unsigned char buffer[5];
	for (size_t i = 0; i < 5; i++) {
		buffer[i] = (char)i;
	}

	LogCtx* ctx = LogOpen(L"\\??\\C:\\ProgramData\\winlog.txt");
	if (ctx)
	{
		LogFormatW(ctx, L"%P\n", 10);
		
		LogFormatW(ctx, L"%S\n", "123");

		LogFormatBufferW(ctx, buffer, 5, 0, 1);
		
		LogClose(ctx);
	}

	DebugFormatBufferA(buffer, 5, 0, 4);
	
	DebugFormatBufferW(buffer, 5, 0, 4);

	//DebugFormatW(L"%P\n", 10);
	//DebugFormatA("%b\n", 3);

	return 0;
}