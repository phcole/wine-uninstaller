#include <windows.h>
#include <objbase.h> 
#include <shlobj.h>
#include <shobjidl.h>
#include <objidl.h>

typedef DWORD WINAPI (*F_CommandLineFromMsiDescriptor)(WCHAR *szDescriptor, WCHAR *szCommandLine, DWORD *pcchCommandLine);

class ShelllinkWrapper
{
private:
    IShellLink *sl;
    IPersistFile *pf;
    HMODULE hm;
    F_CommandLineFromMsiDescriptor pCommandLineFromMsiDescriptor;

public:
    ShelllinkWrapper();
    ShelllinkWrapper(LPCWSTR linkfile);
    ~ShelllinkWrapper();

    BOOL Load(LPCWSTR linkfile);
    HRESULT get_cmdline(LPWSTR szPath, DWORD pathSize, LPWSTR szArgs, DWORD argsSize);
};