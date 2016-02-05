#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>
#include "shelllinkwrapper.h"

BOOL RunApp(LPCWSTR cmd, BOOL wait)
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
        //get_cmdline(szArglist[1], cmd, MAX_PATH, arg, INFOTIPSIZE);
        wprintf(L"%s - %s\n", cmd, arg);
    }
    else
    {
        wprintf(L"no param given.\n");
    }
    RunApp(L"uninstaller.exe", FALSE);
    return 0;
}