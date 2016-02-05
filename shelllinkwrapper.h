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
    ShelllinkWrapper();
    ShelllinkWrapper(LPCWSTR linkfile);
    ~ShelllinkWrapper();

    BOOL Load(LPCWSTR linkfile);
    BOOL Save(LPCWSTR linkfile);
    VOID Release();
    HRESULT get_cmdline(LPWSTR szPath, DWORD pathSize, LPWSTR szArgs, DWORD argsSize);
    BOOL GetWorkDir(LPWSTR szPath, DWORD pathSize);
};