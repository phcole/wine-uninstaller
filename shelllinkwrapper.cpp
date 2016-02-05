#ifndef UNICODE
#define UNICODE
#endif

#include "shelllinkwrapper.h"

ShelllinkWrapper::ShelllinkWrapper()
        : sl(NULL)
        , pf(NULL)
        , hm(NULL)
        , pCommandLineFromMsiDescriptor(NULL)
{
    CoInitialize(NULL);
    this->hm = LoadLibraryW(L"advapi32.dll");
    if (this->hm != NULL)
        this->pCommandLineFromMsiDescriptor = (F_CommandLineFromMsiDescriptor)GetProcAddress(hm, "CommandLineFromMsiDescriptor");
}

ShelllinkWrapper::ShelllinkWrapper(LPCWSTR linkfile)
{
    ShelllinkWrapper();
    Load(linkfile);
}

ShelllinkWrapper::~ShelllinkWrapper()
{
    if (this->pf)
        this->pf->Release();
    if (this->sl)
        this->sl->Release();
    if (this->hm)
        FreeLibrary(this->hm);
    CoUninitialize();
}

BOOL ShelllinkWrapper::Load(LPCWSTR linkfile)
{
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&(this->sl));
    this->sl->QueryInterface(IID_IPersistFile, (void**)&(this->pf));
    this->pf->Load(linkfile, STGM_READ);

    return TRUE;
}

/* gets the target path directly or through MSI */
HRESULT ShelllinkWrapper::get_cmdline(LPWSTR szPath, DWORD pathSize, LPWSTR szArgs, DWORD argsSize)
{
    IShellLinkDataList *dl = NULL;
    EXP_DARWIN_LINK *dar = NULL;
    HRESULT hr;

    szPath[0] = 0;
    szArgs[0] = 0;

    hr = this->sl->GetPath( szPath, pathSize, NULL, SLGP_RAWPATH );
    if (hr == S_OK && szPath[0])
    {
        this->sl->GetArguments( szArgs, argsSize );
        return hr;
    }

    hr = this->sl->QueryInterface( IID_IShellLinkDataList, (LPVOID*) &dl );
    if (FAILED(hr))
        return hr;

    hr = dl->CopyDataBlock( EXP_DARWIN_ID_SIG, (LPVOID*) &dar );
    if (SUCCEEDED(hr))
    {
        WCHAR* szCmdline;
        DWORD cmdSize;

        cmdSize=0;
        hr = this->pCommandLineFromMsiDescriptor( dar->szwDarwinID, NULL, &cmdSize );
        if (hr == ERROR_SUCCESS)
        {
            cmdSize++;
            szCmdline = (WCHAR*)HeapAlloc( GetProcessHeap(), 0, cmdSize*sizeof(WCHAR) );
            hr = this->pCommandLineFromMsiDescriptor( dar->szwDarwinID, szCmdline, &cmdSize );
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
    return hr;
}
