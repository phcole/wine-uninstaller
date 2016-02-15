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
		length = wcslen(str) + 1;
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
	
	m_reg.Open(HKLM, REG_UNINSTALL_ROOT_KEY, TRUE, FALSE);
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
	BOOL found = FALSE;
	WCHAR *ret_val = NULL, *buffer;
	DWORD value_max_length, buffer_real_length = 0, length, match_length = wcslen(match);
	RegistryWrapper subkey;
	for (DWORD i = 0; i < m_size; ++i)
	{
		found = FALSE;
		subkey.Open(m_reg.GetHandle(), m_list[i], FALSE, TRUE);
		subkey.Info(NULL, NULL, NULL, NULL, &value_max_length);
		if (value_max_length > buffer_real_length)
		{
			if (buffer_real_length)
				delete [] buffer;
			buffer_real_length = value_max_length;
			buffer = new WCHAR[buffer_real_length];
		}
		wprintf(L"----%s----\n", m_list[i]);
		length = buffer_real_length;
		if (subkey.Query(L"DisplayName", NULL, (LPBYTE)buffer, &length))
		{
			wprintf(L"DisplayName: %s\n", buffer);
		}
		length = buffer_real_length;
		if (subkey.Query(L"InstallLocation", NULL, (LPBYTE)buffer, &length))
		{
			wprintf(L"InstallLocation: %s\n", buffer);
			if (length > 2)
			{
				if (0 == _wcsnicmp(buffer, match, MIN(wcslen(buffer), match_length)))
					found = TRUE;
			}
		}
		length = buffer_real_length;
		if (subkey.Query(L"UninstallString", NULL, (LPBYTE)buffer, &length))
		{
			wprintf(L"UninstallString: %s\n", buffer, length, wcslen(buffer));
			if (FALSE == found && 2 < length && 0 != wcsnicmp(buffer, L"MsiExec.exe", 11))
			{
				WCHAR *tmp = StrDumpW(buffer);
				PathRemoveArgs(tmp);
				PathUnquoteSpaces(tmp);
				PathRemoveFileSpec(tmp);
				DWORD tmp_length = wcslen(tmp);
				if (0 < tmp_length && 0 == _wcsnicmp(tmp, match, MIN(tmp_length, match_length)))
					found = TRUE;
				delete [] tmp;
			}
			if (found)
			{
				ret_val = StrDumpW(buffer);
				break;
			}
		}
	}
	if (buffer)
		delete [] buffer;
	return ret_val;
}