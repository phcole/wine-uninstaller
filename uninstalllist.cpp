/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "uninstalllist.h"
#include <stdio.h>
#include <shlwapi.h>

#define REG_UNINSTALL_ROOT_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

char* StrDumpA(LPCSTR str, DWORD length = 0)
{
	char* ret = NULL;
	if (str == NULL || str[0] == 0)
		return NULL;
	if ( ! length)
		length = strlen(str) + 1;
	if (0 != str[length - 1])
		++length;
	ret = new char[length];
	memcpy(ret, str, length);
	ret[length - 1] = 0;
	return ret;
}

WCHAR* StrDumpW(LPCWSTR str, DWORD length = 0)
{
	WCHAR* ret = NULL;
	if (str == NULL || str[0] == 0)
		return NULL;
	if ( ! length)
		length = wcslen(str) + 1;
	if (0 != str[length - 1])
		++length;
	ret = new WCHAR[length];
	memcpy(ret, str, length * sizeof(WCHAR));
	ret[length - 1] = 0;
	return ret;
}

inline BOOL StrCompareArrayW(WCHAR *str, WCHAR **array, DWORD count)
{
	for (DWORD i = 0; i < count; ++i)
	{
		fwprintf(stderr, L"APP: compare %s - %s\n", str, array[i]);
		if (0 == _wcsnicmp(str, array[i], MIN(wcslen(str), wcslen(array[i]))))
		{
			return TRUE;
		}
	}
	return FALSE;
}

UninstallList::UninstallList()
		: m_list(NULL)
		, m_size(0)
{
}

UninstallList::~UninstallList()
{
	FreeAppList();
}

VOID UninstallList::GetAppList(HKEY hkey)
{
	WCHAR* buffer;
	DWORD i = 0, max_length, length;
	
	if ( ! m_reg.Open(hkey, REG_UNINSTALL_ROOT_KEY, TRUE, FALSE))
		return;
	if ( ! m_reg.Info(&m_size, &max_length, NULL, NULL, NULL))
		return;
	if (0 == m_size)
		return;
	m_list = new WCHAR*[m_size];
	++max_length;
	buffer = new WCHAR[max_length];
	length = max_length;
	while (m_reg.EnumKey(buffer, &length))
	{
		m_list[i] = StrDumpW(buffer);
		++i;
		length = max_length;
	}
	delete [] buffer;
}

VOID UninstallList::FreeAppList()
{
	for (DWORD i = 0; i < m_size; ++i)
	{
		delete [] m_list[i];
	}
	delete [] m_list;
	m_list = NULL;
	m_size = 0;
}

WCHAR* UninstallList::MatchUninstallerByPath(HKEY root_key, WCHAR** match, DWORD count)
{
	BOOL found = FALSE;
	WCHAR *ret_val = NULL, *buffer;
	DWORD value_max_length, buffer_real_length = 0, length;
	RegistryWrapper subkey;
	GetAppList(root_key);
	for (DWORD i = 0; i < m_size; ++i)
	{
		if ( ! subkey.Open(m_reg.GetHandle(), m_list[i], FALSE, TRUE))
		{
			fwprintf(stderr, L"APP: open error [%s]%d\n", m_list[i], GetLastError());
			continue;
		}
		if ( ! subkey.Info(NULL, NULL, NULL, NULL, &value_max_length))
		{
			fwprintf(stderr, L"APP: Info error [%s]%d\n", m_list[i], GetLastError());
			continue;
		}
		fwprintf(stderr, L"APP: value_max_length %d\n", value_max_length);
		if (value_max_length > buffer_real_length)
		{
			if (buffer_real_length)
				delete [] buffer;
			buffer_real_length = value_max_length;
			buffer = new WCHAR[buffer_real_length];
			memset(buffer, 0, buffer_real_length * sizeof(WCHAR));
		}
		fwprintf(stderr, L"APP: ----%s----\n", m_list[i]);
		length = buffer_real_length;
		if (subkey.Query(L"DisplayName", NULL, (LPBYTE)buffer, &length))
		{
			fwprintf(stderr, L"APP: DisplayName: %s\n", buffer);
		}
		length = buffer_real_length;
		if (subkey.Query(L"InstallLocation", NULL, (LPBYTE)buffer, &length))
		{
			fwprintf(stderr, L"APP: InstallLocation: %s\n", buffer);
			if (length > 2 && StrCompareArrayW(buffer, match, count))
				found = TRUE;
		}
		length = buffer_real_length;
		if (subkey.Query(L"UninstallString", NULL, (LPBYTE)buffer, &length))
		{
			if (FALSE == found && 2 < length && 0 != _wcsnicmp(buffer, L"MsiExec.exe", 11))
			{
				WCHAR *tmp = StrDumpW(buffer);
				if ( ! PathFileExists(tmp))
				{
					PathRemoveArgs(tmp);
					fwprintf(stderr, L"APP: %s\n", tmp);
					PathUnquoteSpaces(tmp);
					fwprintf(stderr, L"APP: %s\n", tmp);
				}
				PathRemoveFileSpec(tmp);
				fwprintf(stderr, L"APP: %s\n", tmp);
				DWORD tmp_length = wcslen(tmp);
				if (0 < tmp_length && StrCompareArrayW(tmp, match, count))
					found = TRUE;
				delete [] tmp;
			}
			if (found)
			{
				if (subkey.IsValueExist(L"QuietUninstallString"))
				{
					WCHAR *quiet = new WCHAR[buffer_real_length];
					length = buffer_real_length;
					if (subkey.Query(L"QuietUninstallString", NULL, (LPBYTE)quiet, &length))
						ret_val = StrDumpW(quiet);
					delete [] quiet;
				}
				else
					ret_val = StrDumpW(buffer);
				break;
			}
		}
	}
	if (buffer)
		delete [] buffer;
	FreeAppList();
	return ret_val;
}

WCHAR* UninstallList::MatchUninstallerByPath(WCHAR** match, DWORD count)
{
	WCHAR* ret = MatchUninstallerByPath(HKLM, match, count);
	if (NULL == ret)
		ret = MatchUninstallerByPath(HKCU, match, count);
	return ret;
}