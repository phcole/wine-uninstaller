#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>
#include "shelllinkwrapper.h"
#include "uninstalllist.h"

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

int WINAPI wWinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPWSTR lpCmdLine,
        int nCmdShow)
{
    WCHAR cmd[MAX_PATH], arg[INFOTIPSIZE];
    WCHAR** list;
    DWORD size;
    LPWSTR *szArglist;
    int nArgs;
    HMODULE hm;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    printf("%s\n", GetCommandLineA());

    UninstallList unist;

    if (nArgs > 1)
    {
        wprintf(L"Args: %s %d\n", szArglist[1], PathFileExists(szArglist[1]));
        ShelllinkWrapper shllnk(szArglist[1]);
        shllnk.GetWorkDir(cmd, MAX_PATH);
        wprintf(L"Workdir: %s\n", cmd);
        unist.ScanForMatch(cmd);
        HRESULT ret = shllnk.get_cmdline(cmd, MAX_PATH, arg, INFOTIPSIZE);
        if (S_OK == ret)
            wprintf(L"cmd: %s - args: %s\n", cmd, arg);
        else
        {
            wprintf(L"ERROR: %d\n", GetLastError());
            RunApp(L"uninstaller.exe", FALSE);
        }
        return 0;
    }
    else
    {
        wprintf(L"no param given.\n");
    }
    return 1;
}