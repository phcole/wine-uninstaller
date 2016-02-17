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
    WCHAR** scan_path;
    int nArgs;
    DWORD scan_path_count = 0;

    printf("CMD ANSI:    %s\n", GetCommandLineA());
    wprintf(L"CMD UNICODE: %s\n", GetCommandLineW());
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (nArgs <= 1)
    {
        wprintf(L"no param given.\n");
        return 1;
    }
    wprintf(L"Args: %s %d\n", szArglist[1], PathFileExists(szArglist[1]));

    ShelllinkWrapper shllnk(szArglist[1]);
    UninstallList unist;

    scan_path = new WCHAR*[2];
    if (shllnk.GetWorkDir(cmd, MAX_PATH))
    {
        scan_path[scan_path_count++] = StrDumpW(cmd, 0);
        wprintf(L"Get path: %s\n", cmd);
    }
    if (shllnk.GetCmd(cmd, MAX_PATH, arg, INFOTIPSIZE) && PathRemoveFileSpec(cmd))
    {
        scan_path[scan_path_count++] = StrDumpW(cmd, 0);
        wprintf(L"Get path: %s\n", cmd);
    }
    if (scan_path_count > 0)
        unins_cmd = unist.MatchUninstallerByPath(scan_path, scan_path_count);
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
    delete [] scan_path;
    return 0;
}