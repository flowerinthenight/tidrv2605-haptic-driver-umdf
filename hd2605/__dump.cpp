#include "__dump.h"
#include <stdlib.h>
#include <strsafe.h>

void L2(wchar_t *pszFunc, wchar_t *pszFormat, ...)
{
	int nSize = 0;
	wchar_t szBuffer[MAX_PATH];
	wchar_t szDump[MAX_PATH * 2];
	va_list valist;
	va_start(valist, pszFormat);
	memset(szBuffer, 0, sizeof(szBuffer));
	nSize = _vsnwprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, pszFormat, valist);
	StringCchPrintf(szDump, MAX_PATH * 2, L"[%d_%s] >>> %s\n", GetCurrentThreadId(), pszFunc, szBuffer);
	OutputDebugString(szDump);
}
