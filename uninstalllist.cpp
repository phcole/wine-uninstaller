#include "uninstalllist.h"
#include <stdio.h>

char* StrDumpA(LPCSTR str, DWORD length = 0)
{
	char* ret = NULL;
	if (str == NULL || str[0] == 0)
		return NULL;
	if ( ! length)
		length = strlen(str);
	if (0 != str[length])
		++length;
	ret = new char[length];
	memcpy(ret, str, length);
	return ret;
}

WCHAR* StrDumpW(LPCWSTR str, DWORD length = 0)
{
	WCHAR* ret = NULL;
	if (str == NULL || str[0] == 0)
		return NULL;
	if ( ! length)
		length = wcslen(str);
	if (0 != str[length])
		++length;
	ret = new WCHAR[length];
	memcpy(ret, str, length * sizeof(WCHAR));
	return ret;
}

UninstallList::UninstallList()
{
	GetAppList();
}

UninstallList::~UninstallList()
{
	FreeAppList();
}

VOID UninstallList::GetAppList()
{
	WCHAR* buffer;
	DWORD i = 0, max_length, length;
	
	m_reg.Open(HKLM, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", TRUE, FALSE);
	m_reg.Info(&m_size, &max_length, NULL, NULL, NULL);
	if (0 == m_size)
		return;
	m_list = new WCHAR*[m_size];
	++max_length;
	buffer = new WCHAR[max_length];
	length = max_length;
	while (m_reg.EnumKey(buffer, &length))
	{
		m_list[i] = StrDumpW(buffer, length);
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

WCHAR* UninstallList::ScanForMatch(LPCWSTR match)
{
	WCHAR* ret_val = NULL,* buffer;
	DWORD count, value_max_length, buffer_length = 0, length;
	RegistryWrapper subkey;
	for (DWORD i = 0; i < m_size; ++i)
	{
		subkey.Open(m_reg.GetHandle(), m_list[i], FALSE, TRUE);
		subkey.Info(NULL, NULL, &count, NULL, &value_max_length);
		if (value_max_length > buffer_length)
		{
			if (buffer_length)
				delete [] buffer;
			buffer_length = value_max_length;
			buffer = new WCHAR[buffer_length];
		}
		wprintf(L"----%s----\n", m_list[i]);
		length = buffer_length;
		if (subkey.Query(L"DisplayName", NULL, (LPBYTE)buffer, &length))
		{
			wprintf(L"DisplayName: %s[%d - %d]\n", buffer, length, wcslen(buffer));
		}
		length = buffer_length;
		if (subkey.Query(L"InstallLocation", NULL, (LPBYTE)buffer, &length))
		{
			wprintf(L"InstallLocation: %s[%d - %d]\n", buffer, length, wcslen(buffer));
		}
		length = buffer_length;
		if (subkey.Query(L"UninstallString", NULL, (LPBYTE)buffer, &length))
		{
			wprintf(L"UninstallString: %s[%d - %d]\n", buffer, length, wcslen(buffer));
		}
	}
	if (buffer)
		delete [] buffer;
	return ret_val;
}