#ifndef UNICODE
#define UNICODE
#endif

#include "shelllinkwrapper.h"
#include <stdio.h>

ShelllinkWrapper::ShelllinkWrapper(LPCWSTR linkfile = NULL)
        : m_sl(NULL)
        , m_pf(NULL)
        , m_hm(NULL)
        , m_CommandLineFromMsiDescriptor(NULL)
{
    HRESULT ret = CoInitialize(NULL);
    m_hm = LoadLibraryW(L"advapi32.dll");
    if (m_hm != NULL)
        m_CommandLineFromMsiDescriptor = (F_CommandLineFromMsiDescriptor)GetProcAddress(m_hm, "CommandLineFromMsiDescriptor");
    if (linkfile)
        Load(linkfile);
}

ShelllinkWrapper::~ShelllinkWrapper()
{
    Release();
    if (m_hm)
        FreeLibrary(m_hm);
    CoUninitialize();
}

BOOL ShelllinkWrapper::Load(LPCWSTR linkfile)
{
    Release();
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&m_sl);
    m_sl->QueryInterface(IID_IPersistFile, (void**)&m_pf);
    m_pf->Load(linkfile, STGM_READ);

    return TRUE;
}

BOOL ShelllinkWrapper::Save(LPCWSTR linkfile)
{
    m_pf->Save(linkfile, TRUE);

    return TRUE;
}

VOID ShelllinkWrapper::Release()
{
    if (m_pf)
        m_pf->Release();
    if (m_sl)
        m_sl->Release();
}

/* gets the target path directly or through MSI */
BOOL ShelllinkWrapper::GetCmd(LPWSTR szPath, DWORD pathSize, LPWSTR szArgs, DWORD argsSize)
{
    IShellLinkDataList *dl = NULL;
    EXP_DARWIN_LINK *dar = NULL;
    HRESULT hr;

    szPath[0] = 0;
    szArgs[0] = 0;

    hr = m_sl->GetPath( szPath, pathSize, NULL, SLGP_RAWPATH );
    if (hr == S_OK && szPath[0])
    {
        m_sl->GetArguments(szArgs, argsSize);
        return TRUE;
    }

    hr = m_sl->QueryInterface( IID_IShellLinkDataList, (LPVOID*) &dl );
    if (FAILED(hr))
        return FALSE;

    hr = dl->CopyDataBlock( EXP_DARWIN_ID_SIG, (LPVOID*) &dar );
    if (SUCCEEDED(hr))
    {
        WCHAR* szCmdline;
        DWORD cmdSize;

        wprintf(L"MSI - %s\n", dar->szwDarwinID);

        cmdSize=0;
        hr = m_CommandLineFromMsiDescriptor( dar->szwDarwinID, NULL, &cmdSize );
        if (hr == ERROR_SUCCESS)
        {
            cmdSize++;
            szCmdline = (WCHAR*)HeapAlloc( GetProcessHeap(), 0, cmdSize*sizeof(WCHAR) );
            hr = m_CommandLineFromMsiDescriptor( dar->szwDarwinID, szCmdline, &cmdSize );
            if (hr == ERROR_SUCCESS)
            {
                WCHAR *s, *d;
                int bcount = 0;
                BOOL in_quotes = FALSE;

                /* Extract the application path */
                s=szCmdline;
                d=szPath;
                while (*s)
                {
                    if ((*s==0x0009 || *s==0x0020) && !in_quotes)
                    {
                        /* skip the remaining spaces */
                        do {
                            s++;
                        } while (*s==0x0009 || *s==0x0020);
                        break;
                    }
                    else if (*s==0x005c)
                    {
                        /* '\\' */
                        *d++=*s++;
                        bcount++;
                    }
                    else if (*s==0x0022)
                    {
                        /* '"' */
                        if ((bcount & 1)==0)
                        {
                            /* Preceded by an even number of '\', this is
                             * half that number of '\', plus a quote which
                             * we erase.
                             */
                            d-=bcount/2;
                            in_quotes=!in_quotes;
                            s++;
                        }
                        else
                        {
                            /* Preceded by an odd number of '\', this is
                             * half that number of '\' followed by a '"'
                             */
                            d=d-bcount/2-1;
                            *d++='"';
                            s++;
                        }
                        bcount=0;
                    }
                    else
                    {
                        /* a regular character */
                        *d++=*s++;
                        bcount=0;
                    }
                    if ((d-szPath) == pathSize)
                    {
                        /* Keep processing the path till we get to the
                         * arguments, but 'stand still'
                         */
                        d--;
                    }
                }
                /* Close the application path */
                *d=0;

                lstrcpynW(szArgs, s, argsSize);
            }
            HeapFree( GetProcessHeap(), 0, szCmdline );
        }
        LocalFree( dar );
    }
    dl->Release();
    return SUCCEEDED(hr);
}

BOOL ShelllinkWrapper::GetWorkDir(LPWSTR szPath, DWORD pathSize)
{
    return SUCCEEDED(m_sl->GetWorkingDirectory(szPath, pathSize));
}
