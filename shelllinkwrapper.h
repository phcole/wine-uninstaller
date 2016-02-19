/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef _F_SHELLLINKWRAPPER_H_
#define _F_SHELLLINKWRAPPER_H_

#include <windows.h>
#include <objbase.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <objidl.h>

typedef DWORD WINAPI (*F_CommandLineFromMsiDescriptor)(WCHAR *szDescriptor, WCHAR *szCommandLine, DWORD *pcchCommandLine);

class ShelllinkWrapper
{
private:
    IShellLink *m_sl;
    IPersistFile *m_pf;
    HMODULE m_hm;
    F_CommandLineFromMsiDescriptor m_CommandLineFromMsiDescriptor;

public:
    ShelllinkWrapper(void);
    ~ShelllinkWrapper(void);

    BOOL Load(LPCWSTR linkfile);
    BOOL Save(LPCWSTR linkfile);
    VOID Release(void);
    BOOL GetCmd(LPWSTR szPath, DWORD pathSize, LPWSTR szArgs, DWORD argsSize);
    BOOL GetWorkDir(LPWSTR szPath, DWORD pathSize);
};

#endif//_F_SHELLLINKWRAPPER_H_