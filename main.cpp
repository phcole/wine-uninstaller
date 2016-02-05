#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>
#include "shelllinkwrapper.h"

BOOL RunApp(LPCWSTR cmd, BOOL wait = FALSE)
{
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    WCHAR szcmd[MAX_PATH] = {0};
    si.cb = sizeof(si);
    wcscpy(szcmd, cmd);
    if ( ! CreateProcess(NULL, szcmd, NULL, NULL, FALSE, 0, NULL, NULL, &si , &pi))
        return FALSE;

    if (wait)
        WaitForSingleObject(pi.hProcess,INFINITE);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return TRUE;
}

int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    WCHAR cmd[MAX_PATH], arg[INFOTIPSIZE];
    LPWSTR *szArglist;
    int nArgs;
    HMODULE hm;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    if (nArgs > 1)
    {
        wprintf(L"%s %d\n", szArglist[1], PathFileExists(szArglist[1]));
        ShelllinkWrapper wrapper(szArglist[1]);
        HRESULT ret = wrapper.get_cmdline(cmd, MAX_PATH, arg, INFOTIPSIZE);
        if (S_OK == ret)
            wprintf(L"%s - %s\n", cmd, arg);
        else
        {
            wprintf(L"ERROR: %d\n", GetLastError());
            RunApp(L"uninstaller.exe", FALSE);
        }
        wrapper.GetWorkDir(cmd, MAX_PATH);
        wprintf(L"%s\n", cmd);
        return 0;
    }
    else
    {
        wprintf(L"no param given.\n");
    }
    return 1;
}