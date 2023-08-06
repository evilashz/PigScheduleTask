#include <stdio.h>
#include <Windows.h>
#include "SchRPC_h.h"

#pragma comment(lib, "rpcrt4.lib")

const RPC_WSTR RPC_UUID = (RPC_WSTR)L"86D35949-83C9-4044-B424-DB363231FD0C";
const RPC_WSTR InterfaceAddress = (RPC_WSTR)L"\\pipe\\atsvc";

#define TASK_CREATE 2
#define TASK_LOGON_NONE 0

typedef RPC_STATUS(WINAPI* MyRpcStringBindingComposeW)(
	RPC_WSTR ObjUuid,
	RPC_WSTR ProtSeq,
	RPC_WSTR NetworkAddr,
	RPC_WSTR Endpoint,
	RPC_WSTR Options,
	RPC_WSTR* StringBinding
);
typedef RPC_STATUS(WINAPI* MyRpcBindingFromStringBindingW)(
	RPC_WSTR           StringBinding,
	RPC_BINDING_HANDLE* Binding
);
typedef RPC_STATUS(WINAPI* MyRpcBindingSetAuthInfoExA)(
	RPC_BINDING_HANDLE       Binding,
	RPC_CSTR                 ServerPrincName,
	unsigned long            AuthnLevel,
	unsigned long            AuthnSvc,
	RPC_AUTH_IDENTITY_HANDLE AuthIdentity,
	unsigned long            AuthzSvc,
	RPC_SECURITY_QOS* SecurityQos
);
typedef RPC_STATUS(WINAPI* MyRpcStringFreeW)(
	RPC_WSTR* String
);

MyRpcStringBindingComposeW nRpcStringBindingComposeW;
MyRpcBindingFromStringBindingW nRpcBindingFromStringBindingW;
MyRpcBindingSetAuthInfoExA nRpcBindingSetAuthInfoExA;
MyRpcStringFreeW nRpcStringFreeW;

//10 min time triger
static const wchar_t* xml =
L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>\n"
L"<Task version=\"1.3\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\n"
L"  <RegistrationInfo>\n"
L"    <Author>Microsoft Corporation</Author>\n"
L"    <Description>WPS Office Update Task.</Description>\n"
L"    <URI>\\Microsoft Corporation</URI>\n"
L"  </RegistrationInfo>\n"
L"  <Triggers>\n"
L"    <TimeTrigger id=\"Trigger1\">\n"
L"      <Repetition>\n"
L"        <Interval>PT10M</Interval>\n"
L"        <StopAtDurationEnd>false</StopAtDurationEnd>\n"
L"      </Repetition>\n"
L"      <StartBoundary>2005-01-01T12:05:00</StartBoundary>\n"
L"      <Enabled>true</Enabled>\n"
L"    </TimeTrigger>\n"
L"  </Triggers>\n"
L"  <Settings>\n"
L"    <MultipleInstancesPolicy>IgnoreNew</MultipleInstancesPolicy>\n"
L"    <DisallowStartIfOnBatteries>true</DisallowStartIfOnBatteries>\n"
L"    <StopIfGoingOnBatteries>false</StopIfGoingOnBatteries>\n"
L"    <AllowHardTerminate>true</AllowHardTerminate>\n"
L"    <StartWhenAvailable>false</StartWhenAvailable>\n"
L"    <RunOnlyIfNetworkAvailable>false</RunOnlyIfNetworkAvailable>\n"
L"    <IdleSettings>\n"
L"      <Duration>PT10M</Duration>\n"
L"		<WaitTimeout>PT1H</WaitTimeout>\n"
L"      <StopOnIdleEnd>false</StopOnIdleEnd>\n"
L"      <RestartOnIdle>false</RestartOnIdle>\n"
L"    </IdleSettings>\n"
L"    <AllowStartOnDemand>true</AllowStartOnDemand>\n"
L"    <Enabled>true</Enabled>\n"
L"    <Hidden>false</Hidden>\n"
L"    <RunOnlyIfIdle>false</RunOnlyIfIdle>\n"
L"    <WakeToRun>false</WakeToRun>\n"
L"    <ExecutionTimeLimit>PT72H</ExecutionTimeLimit>\n"
L"    <Priority>7</Priority>\n"
L"  </Settings>\n"
L"  <Actions Context=\"Author\">\n"
L"    <Exec>\n"
L"      <Command>%s</Command>\n"
L"    </Exec>\n"
L"  </Actions>\n"
L"  <Principals>\n"
L"    <Principal id=\"Author\">\n"
L"      <UserId>S-1-5-21-1038597295-145889836-4018320550-500</UserId>\n"
L"      <LogonType>InteractiveToken</LogonType>\n"
L"      <RunLevel>LeastPrivilege</RunLevel>\n"
L"    </Principal>\n"
L"  </Principals>\n"
L"</Task>\n";



char* getSid() {

	char userName[260] = "";
	char sid[260] = "";
	DWORD nameSize = sizeof(userName);
	GetUserName((LPWSTR)userName, &nameSize);


	char userSID[260] = "";
	char userDomain[260] = "";
	DWORD sidSize = sizeof(userSID);
	DWORD domainSize = sizeof(userDomain);


	SID_NAME_USE snu;
	LookupAccountName(NULL,
		(LPCWSTR)userName,
		(PSID)userSID,
		&sidSize,
		(LPWSTR)userDomain,
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
handle_t BindtoRpc()
{
	wchar_t buffer[100];
	RPC_WSTR StringBinding;
	handle_t BindingHandle;
	RPC_SECURITY_QOS SecurityQOS = { 0 };

	RpcStringBindingComposeW(RPC_UUID, (RPC_WSTR)L"ncacn_np", (RPC_WSTR)L"localhost", InterfaceAddress, NULL, &StringBinding);

	RpcBindingFromStringBindingW(StringBinding, &BindingHandle);

	SecurityQOS.Version = 1;
	SecurityQOS.ImpersonationType = RPC_C_IMP_LEVEL_IMPERSONATE;
	SecurityQOS.Capabilities = RPC_C_QOS_CAPABILITIES_DEFAULT;
	SecurityQOS.IdentityTracking = RPC_C_QOS_IDENTITY_STATIC;
	RpcBindingSetAuthInfoExA(BindingHandle, 0, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, 0xA, 0, RPC_C_AUTHZ_NONE, &SecurityQOS);

	RpcStringFreeW(&StringBinding);

	return(BindingHandle);
}


BOOL CreateTask(handle_t handle, wchar_t* ProgramPath, wchar_t* wszProgramArgs) {

	wchar_t xmlBuffer[4096];
	wchar_t* actualPath = NULL;
	TASK_XML_ERROR_INFO* errorInfo = NULL;

	char* sid = getSid();

	wchar_t wszClassName[256];

	memset(wszClassName, 0, sizeof(wszClassName));

	MultiByteToWideChar(CP_ACP, 0, sid, strlen(sid) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));

	swprintf(xmlBuffer, 4096, xml, ProgramPath);

	HRESULT status = SchRpcRegisterTask(handle, L"\\test", xmlBuffer, TASK_CREATE, NULL, 0, 0, NULL, &actualPath, &errorInfo);

	if (status == S_OK)
	{
		return TRUE;
	}

	return FALSE;
}

int wmain(int argc, wchar_t* argv[]) {


	handle_t handle = BindtoRpc();

	BOOL isSuccess = CreateTask(handle, argv[1], NULL);

	if (isSuccess == TRUE)
	{
		printf("add success");
	}
	else {
		printf("false");
	}

	return 0;
}


void __RPC_FAR* __RPC_USER midl_user_allocate(size_t cBytes)
{
	return((void __RPC_FAR*) malloc(cBytes));
}

void __RPC_USER midl_user_free(void __RPC_FAR* p)
{
	free(p);
}