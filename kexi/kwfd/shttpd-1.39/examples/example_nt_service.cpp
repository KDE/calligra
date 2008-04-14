/* WinNT service example. You may use Makc <makc.the.great@gmail.com> for credits ;) */

#define SHTTPD_SERVICE_NAME "shttpd"


#include <windows.h>
#include <winsock.h>
#define snprintf _snprintf
#pragma comment(lib,"ws2_32")
#pragma comment(lib,"advapi32")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "shttpd.h"


/* Windows service C++ implementation
   based on MSDN sample by Nigel Thompson (Microsoft Developer Network Technology Group) November 1995 */

class CNTService
{
public:
    BOOL Init();
    void Run();

    CNTService(const char* szServiceName);
    ~CNTService();

    BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();

    BOOL StartService();
    void SetStatus(DWORD dwState);
    BOOL Initialize();

    static void WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI Handler(DWORD dwOpcode);

    char m_szServiceName[64];
    int m_iMajorVersion;
    int m_iMinorVersion;
    SERVICE_STATUS_HANDLE m_hServiceStatus;
    SERVICE_STATUS m_Status;
    BOOL m_bIsRunning;

    static CNTService* m_pThis; // this code is older than HandlerEx :(

private:
    HANDLE m_hEventSource;

};



static int
index_html(struct shttpd_arg_t *arg)
//index_html(struct shttpd_callback_arg *arg)
{
	int		n;

	n = snprintf(arg->buf, arg->buflen, "%s",
	    "HTTP/1.1 200 OK\r\n"
	    "Content-Type: text/html\r\n"
	    "\r\n"
	    "<html><body>Welcome. This is shttpd "
	    "Windows NT service example. "
	    "For more info, visit <a href=\"http://shttpd.sf.net\">"
	    "shttpd.sf.net</a></body></html>");
	arg->last = 1; // ******** inserted by ml
	return (n);
}

void CNTService::Run()
{
    int sock;
    struct  shttpd_ctx	*ctx;

    /* Initialize and setup URLs we gonna serve */
    ctx = shttpd_init(NULL,
        "aliases", "c:\\/->/my_c,/etc/->/my_etc",
	"document_root", ".", NULL);
    /* Let pass 'data' variable to callback, user can change it */
    shttpd_register_url(ctx,"/", &index_html, NULL); //*********** changed ML 
	
    /* Open listening socket */
    sock = shttpd_open_port(9000);//*********** changed ML
    shttpd_listen(ctx, sock);	  //*********** changed ML

    /* Serve connections until SERVICE_CONTROL_STOP received */
    while (m_bIsRunning)
        shttpd_poll(ctx,200); //********* changed ML 

    /* Clean up */
   shttpd_fini(ctx);
}



BOOL CNTService::Init()
{

    return TRUE;
}

int main(int argc, char* argv[])
{
   int iCode = 1;
   BOOL bRun = FALSE;
   CNTService ntService (SHTTPD_SERVICE_NAME);

   if (ntService.IsInstalled()) {
       // uninstall or run
       bRun = TRUE;
       if (argc > 1)
           if (_stricmp(argv[1], "-u") == 0) {
               bRun = FALSE;
               if (ntService.Uninstall())
                   iCode = 0;
           }
   } else {
       // install before run
       if (ntService.Install()) bRun = TRUE;
   }

   if (bRun) {
       if (ntService.StartService())
           iCode = 0;
       else if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
           printf("Service installed; to start it, type this command: net start %s\n\n",
                   ntService.m_szServiceName );
           iCode = 0;
       }
   }

   return iCode;
}


/* CNTService class guts
   Trust me, you don't want to scroll down :( */

CNTService* CNTService::m_pThis = NULL;

CNTService::CNTService(const char* szServiceName)
{
    m_pThis = this;

    // set default service name
    strncpy(m_szServiceName, szServiceName, sizeof(m_szServiceName)-1);
    m_hEventSource = NULL;

    // set initial service status 
    m_hServiceStatus = NULL;
    m_Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_Status.dwCurrentState = SERVICE_STOPPED;
    m_Status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    m_Status.dwWin32ExitCode = 0;
    m_Status.dwServiceSpecificExitCode = 0;
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    m_bIsRunning = FALSE;
}

CNTService::~CNTService()
{
    if (m_hEventSource) ::DeregisterEventSource(m_hEventSource);
}

BOOL CNTService::IsInstalled()
{
    BOOL bResult = FALSE;
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (hSCM) {
        SC_HANDLE hService = ::OpenService(hSCM,
                                           m_szServiceName,
                                           SERVICE_QUERY_CONFIG);
        if (hService) {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

BOOL CNTService::Install()
{
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return FALSE;

    char szFilePath[FILENAME_MAX];
    ::GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));

    SC_HANDLE hService = ::CreateService(hSCM,
                                         m_szServiceName,
                                         m_szServiceName,
                                         SERVICE_ALL_ACCESS,
                                         SERVICE_WIN32_OWN_PROCESS,
                                         SERVICE_AUTO_START, // start condition
                                         SERVICE_ERROR_NORMAL,
                                         szFilePath,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL);
    if (!hService) {
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }

    char szKey[256]; HKEY hKey = NULL;
    strcpy(szKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
    strcat(szKey, m_szServiceName);
    if (::RegCreateKey(HKEY_LOCAL_MACHINE, szKey, &hKey) != ERROR_SUCCESS) {
        ::CloseServiceHandle(hService);
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }

    ::RegSetValueEx(hKey,
                    "EventMessageFile",
                    0,
                    REG_EXPAND_SZ, 
                    (CONST BYTE*)szFilePath,
                   (int) strlen(szFilePath) + 1);     

    DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
    ::RegSetValueEx(hKey,
                    "TypesSupported",
                    0,
                    REG_DWORD,
                    (CONST BYTE*)&dwData,
                     sizeof(DWORD));
    ::RegCloseKey(hKey);

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

BOOL CNTService::Uninstall()
{
    SC_HANDLE hSCM = ::OpenSCManager(NULL, // local machine
                                     NULL, // ServicesActive database
                                     SC_MANAGER_ALL_ACCESS); // full access
    if (!hSCM) return FALSE;

    BOOL bResult = FALSE;
    SC_HANDLE hService = ::OpenService(hSCM,
                                       m_szServiceName,
                                       DELETE);
    if (hService) {
        if (::DeleteService(hService)) bResult = TRUE;
        ::CloseServiceHandle(hService);
    }
    ::CloseServiceHandle(hSCM);
    return bResult;
}

BOOL CNTService::StartService()
{
    SERVICE_TABLE_ENTRY st[] = {
        {m_szServiceName, ServiceMain},
        {NULL, NULL}
    };

    return ::StartServiceCtrlDispatcher(st);
}

void CNTService::SetStatus(DWORD dwState)
{
    m_Status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_Status);
}

BOOL CNTService::Initialize()
{
    SetStatus(SERVICE_START_PENDING);

    BOOL bResult = Init(); // actual initialization -> apollo.cpp
    
    m_Status.dwWin32ExitCode = GetLastError();
    m_Status.dwCheckPoint = 0;
    m_Status.dwWaitHint = 0;
    if (!bResult) {
        SetStatus(SERVICE_STOPPED); return FALSE;    
    }

    SetStatus(SERVICE_RUNNING); return TRUE;
}

void CNTService::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    CNTService* pService = m_pThis;
    
    pService->m_Status.dwCurrentState = SERVICE_START_PENDING;
    pService->m_hServiceStatus = RegisterServiceCtrlHandler(pService->m_szServiceName, Handler);
    if (pService->m_hServiceStatus != NULL) {
        if (pService->Initialize()) {
            pService->m_bIsRunning = TRUE;
            pService->m_Status.dwWin32ExitCode = 0;
            pService->m_Status.dwCheckPoint = 0;
            pService->m_Status.dwWaitHint = 0;
            pService->Run();
        }
        pService->SetStatus(SERVICE_STOPPED);
    }
}

void CNTService::Handler(DWORD dwOpcode)
{
    CNTService* pService = m_pThis;
    
    if ((dwOpcode == SERVICE_CONTROL_STOP) ||
        (dwOpcode == SERVICE_CONTROL_SHUTDOWN))
    {
        pService->SetStatus(SERVICE_STOP_PENDING);
        pService->m_bIsRunning = FALSE; // SERVICE_STOPPED set when Run returns in ServiceMain (above)
    }
    ::SetServiceStatus(pService->m_hServiceStatus, &pService->m_Status);
}
