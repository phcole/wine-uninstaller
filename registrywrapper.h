/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef _F_REGISTRYWRAPPER_H_
#define _F_REGISTRYWRAPPER_H_

#include <windows.h>
#include <tchar.h>

#define HKCR HKEY_CLASSES_ROOT
#define HKCC HKEY_CURRENT_CONFIG
#define HKCU HKEY_CURRENT_USER
#define HKLM HKEY_LOCAL_MACHINE
#define HKUS HKEY_USERS

class RegistryWrapper
{
private:
	HKEY m_key;
	DWORD m_enum_index;

public:
	RegistryWrapper(void);
	~RegistryWrapper(void);

	BOOL Open(HKEY main, LPCTSTR key, BOOL create_if_none, BOOL read_only);
	VOID Close(void);
	HKEY GetHandle(void);

	BOOL Info(LPDWORD subkey_count, LPDWORD subkey_max_length, LPDWORD value_count, LPDWORD value_name_max_length,
			LPDWORD value_max_length);
	DWORD Query(LPCTSTR name, LPDWORD type, LPBYTE value, LPDWORD length);

	VOID EnumReset(void);
	BOOL EnumKey(LPTSTR name, LPDWORD length);
};

#endif//_F_REGISTRYWRAPPER_H_