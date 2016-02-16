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
    LPWSTR *szArglist, unins_cmd = NULL;
    int nArgs;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    printf("CMD ANSI:    %s\n", GetCommandLineA());
    wprintf(L"CMD UNICODE: %s\n", GetCommandLineW());

    UninstallList unist;

    if (nArgs <= 1)
    {
        wprintf(L"no param given.\n");
        return 1;
    }
    wprintf(L"Args: %s %d\n", szArglist[1], PathFileExists(szArglist[1]));
    ShelllinkWrapper shllnk(szArglist[1]);

    if (NULL == unins_cmd && shllnk.GetWorkDir(cmd, MAX_PATH))
    {
        wprintf(L"Workdir: %s\n", cmd);
        unins_cmd = unist.ScanForMatch(cmd);
    }
    if (NULL == unins_cmd && shllnk.GetCmd(cmd, MAX_PATH, arg, INFOTIPSIZE))
    {
        wprintf(L"cmd: %s\n", cmd);
        if (PathRemoveFileSpec(cmd))
            unins_cmd = unist.ScanForMatch(cmd);
    }
    if (unins_cmd)
    {
        wprintf(L"FOUND: %s\n", unins_cmd);
        RunApp(unins_cmd, TRUE);
        delete [] unins_cmd;
    }
    else
    {
        wprintf(L"ERROR: %d\n", GetLastError());
        RunApp(L"uninstaller.exe", TRUE);
    }
    return 0;
}