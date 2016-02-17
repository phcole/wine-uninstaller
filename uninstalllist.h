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

	VOID GetAppList();
	VOID FreeAppList();

public:
	UninstallList();
	~UninstallList();

	WCHAR* MatchUninstallerByPath(WCHAR** match, DWORD count);
};

#endif//_F_SHELLLINKWRAPPER_H_