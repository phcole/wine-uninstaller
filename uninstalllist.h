/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef _F_UNINSTALL_LIST_H_
#define _F_UNINSTALL_LIST_H_

#include <windows.h>
#include "registrywrapper.h"

char* StrDumpA(LPCSTR str, DWORD length);
WCHAR* StrDumpW(LPCWSTR str, DWORD length);

class UninstallList
{
private:
	WCHAR** m_list;
	DWORD m_size;
	RegistryWrapper m_reg;

	VOID GetAppList(HKEY hkey);
	VOID FreeAppList(void);

	WCHAR* MatchUninstallerByPath(HKEY root_key, WCHAR** match, DWORD count);

public:
	UninstallList(void);
	~UninstallList(void);

	WCHAR* MatchUninstallerByPath(WCHAR** match, DWORD count);
};

#endif//_F_SHELLLINKWRAPPER_H_