/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>
#include <tlhelp32.h>
#include "shelllinkwrapper.h"
#include "uninstalllist.h"

HANDLE FindSubApp(DWORD pid)
{
    PROCESSENTRY32 pe32 = {0};
    HANDLE ret = NULL, process_snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == process_snap)
    {
        wprintf(L"APP: CreateToolhelp32Snapshot error[%d]\n", GetLastError());
        return NULL;
    }
    pe32.dwSize = sizeof(pe32);
    BOOL more = Process32First(process_snap, &pe32);
    while (more)
    {
        if (pe32.th32ParentProcessID == pid)
        {
            ret = OpenProcess(SYNCHRONIZE, FALSE, pe32.th32ProcessID);
            break;
        }
        more = Process32Next(process_snap, &pe32);
    }
    CloseHandle(process_snap);
    return ret;
}

BOOL RunApp(LPCWSTR cmd, BOOL wait = FALSE)
{
    HANDLE h;
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    WCHAR szcmd[MAX_PATH] = {0};
    si.cb = sizeof(si);
    wcscpy(szcmd, cmd);
    if ( ! CreateProcess(NULL, szcmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        return FALSE;
    CloseHandle(pi.hThread);
    if (wait)
    {
        DWORD count = 0;
        WaitForSingleObject(pi.hProcess, INFINITE);
        while (count < 10)
        {
            h = FindSubApp(pi.dwProcessId);
            if (h == NULL)
            {
                ++count;
                Sleep(100);
                continue;
            }
            WaitForSingleObject(h, INFINITE);
            CloseHandle(h);
        }
    }
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
    WCHAR **scan_path, *link_file;
    int nArgs;
    DWORD scan_path_count = 0;


    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (nArgs <= 1)
    {
        wprintf(L"APP: no param given.\n");
        return 1;
    }
    link_file = StrDumpW(szArglist[1], 0);
    LocalFree(szArglist);
    if (L'\"' == link_file[0])
        PathUnquoteSpaces(link_file);

    if ( ! PathFileExists(link_file))
    {
        wprintf(L"APP: Link file do not exists.\n");
        return 1;
    }

    ShelllinkWrapper shllnk;
    UninstallList unist;

    if ( ! shllnk.Load(link_file))
    {
        wprintf(L"APP: Error when loading link file.[%d]\n", GetLastError());
        return 1;
    }

    scan_path = new WCHAR*[2];
    if (shllnk.GetWorkDir(cmd, MAX_PATH) && 0 != cmd[0])
    {
        scan_path[scan_path_count++] = StrDumpW(cmd, 0);
        wprintf(L"APP: Get path: %s\n", cmd);
    }
    if (shllnk.GetCmd(cmd, MAX_PATH, arg, INFOTIPSIZE) && 0 != cmd[0] && PathRemoveFileSpec(cmd))
    {
        scan_path[scan_path_count++] = StrDumpW(cmd, 0);
        wprintf(L"APP: Get path: %s\n", cmd);
    }
    if (scan_path_count > 0)
        unins_cmd = unist.MatchUninstallerByPath(scan_path, scan_path_count);
    if (unins_cmd)
    {
        wprintf(L"APP: FOUND: %s\n", unins_cmd);
        RunApp(unins_cmd, TRUE);
        delete [] unins_cmd;
    }
    else
    {
        wprintf(L"APP: ERROR: %d\n", GetLastError());
        RunApp(L"uninstaller.exe", TRUE);
    }
    delete [] scan_path;

    if (PathFileExists(link_file))
        return 1;
    else
        return 0;
}