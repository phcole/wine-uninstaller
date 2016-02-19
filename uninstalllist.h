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