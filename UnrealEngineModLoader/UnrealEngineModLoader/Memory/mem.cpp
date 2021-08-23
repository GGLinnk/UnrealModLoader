#include "mem.h"
#include <tchar.h>
#include <process.h>

bool Read(void* address, void* buffer, unsigned long long size)
{
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _getpid());
	return ReadProcessMemory(hProcess, address, buffer, size, nullptr);
}

namespace MEM
{
	HWND FindWindow(DWORD pid, wchar_t const* className)
	{
		HWND hCurWnd = GetTopWindow(0);
		while (hCurWnd != NULL)
		{
			DWORD cur_pid;
			DWORD dwTheardId = GetWindowThreadProcessId(hCurWnd, &cur_pid);

			if (cur_pid == pid)
			{
				if (IsWindowVisible(hCurWnd) != 0)
				{
					TCHAR szClassName[256];
					GetClassName(hCurWnd, szClassName, 256);
					if (_tcscmp(szClassName, className) == 0)
					{
						return hCurWnd;
					}
				}
			}
			hCurWnd = GetNextWindow(hCurWnd, GW_HWNDNEXT);
		}
		return NULL;
	}
};