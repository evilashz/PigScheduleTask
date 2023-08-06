

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for SchRPC.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __SchRPC_h_h__
#define __SchRPC_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "ms-dtyp.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ITaskSchedulerService_INTERFACE_DEFINED__
#define __ITaskSchedulerService_INTERFACE_DEFINED__

/* interface ITaskSchedulerService */
/* [unique][version][uuid] */ 


enum credFlag
    {
        credFlagDefault	= 0x1
    } ;
typedef struct _TASK_USER_CRED
    {
    /* [string] */ const wchar_t *userId;
    /* [string] */ const wchar_t *password;
    DWORD flags;
    } 	TASK_USER_CRED;

typedef struct _TASK_XML_ERROR_INFO
    {
    DWORD line;
    DWORD column;
    /* [string] */ wchar_t *node;
    /* [string] */ wchar_t *value;
    } 	TASK_XML_ERROR_INFO;

typedef struct _TASK_XML_ERROR_INFO *PTASK_XML_ERROR_INFO;

typedef /* [string] */ wchar_t **TASK_NAMES;

HRESULT SchRpcHighestVersion( 
    /* [in] */ handle_t IDL_handle,
    /* [out] */ DWORD *pVersion);

HRESULT SchRpcRegisterTask( 
    /* [in] */ handle_t IDL_handle,
    /* [unique][string][in] */ const wchar_t *path,
    /* [string][in] */ const wchar_t *xml,
    /* [in] */ DWORD flags,
    /* [unique][string][in] */ const wchar_t *sddl,
    /* [in] */ DWORD logonType,
    /* [in] */ DWORD cCreds,
    /* [unique][size_is][in] */ const TASK_USER_CRED *pCreds,
    /* [string][out] */ wchar_t **pActualPath,
    /* [out] */ PTASK_XML_ERROR_INFO *pErrorInfo);

HRESULT SchRpcRetrieveTask( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [string][in] */ const wchar_t *lpcwszLanguagesBuffer,
    /* [in] */ unsigned long *pulNumLanguages,
    /* [string][out] */ wchar_t **pXml);

HRESULT SchRpcCreateFolder( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [unique][string][in] */ const wchar_t *sddl,
    /* [in] */ DWORD flags);

HRESULT SchRpcSetSecurity( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [string][in] */ const wchar_t *sddl,
    /* [in] */ DWORD flags);

HRESULT SchRpcGetSecurity( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [in] */ DWORD securityInformation,
    /* [string][out] */ wchar_t **sddl);

HRESULT SchRpcEnumFolders( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [in] */ DWORD flags,
    /* [out][in] */ DWORD *pStartIndex,
    /* [in] */ DWORD cRequested,
    /* [out] */ DWORD *pcNames,
    /* [size_is][size_is][string][out] */ TASK_NAMES *pNames);

HRESULT SchRpcEnumTasks( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [in] */ DWORD flags,
    /* [out][in] */ DWORD *startIndex,
    /* [in] */ DWORD cRequested,
    /* [out] */ DWORD *pcNames,
    /* [size_is][size_is][string][out] */ TASK_NAMES *pNames);

HRESULT SchRpcEnumInstances( 
    /* [in] */ handle_t IDL_handle,
    /* [unique][string][in] */ const wchar_t *path,
    /* [in] */ DWORD flags,
    /* [out] */ DWORD *pcGuids,
    /* [size_is][size_is][out] */ GUID **pGuids);

HRESULT SchRpcGetInstanceInfo( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ GUID guid,
    /* [string][out] */ wchar_t **pPath,
    /* [out] */ DWORD *pState,
    /* [string][out] */ wchar_t **pCurrentAction,
    /* [string][out] */ wchar_t **pInfo,
    /* [out] */ DWORD *pcGroupInstances,
    /* [size_is][size_is][out] */ GUID **pGroupInstances,
    /* [out] */ DWORD *pEnginePID);

HRESULT SchRpcStopInstance( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ GUID guid,
    /* [in] */ DWORD flags);

HRESULT SchRpcStop( 
    /* [in] */ handle_t IDL_handle,
    /* [unique][string][in] */ const wchar_t *path,
    /* [in] */ DWORD flags);

HRESULT SchRpcRun( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [in] */ DWORD cArgs,
    /* [unique][size_is][string][in] */ const wchar_t **pArgs,
    /* [in] */ DWORD flags,
    /* [in] */ DWORD sessionId,
    /* [string][unique][in] */ const wchar_t *user,
    /* [out] */ GUID *pGuid);

HRESULT SchRpcDelete( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [in] */ DWORD flags);

HRESULT SchRpcRename( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [string][in] */ const wchar_t *newName,
    /* [in] */ DWORD flags);

HRESULT SchRpcScheduledRuntimes( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [unique][in] */ PSYSTEMTIME start,
    /* [unique][in] */ PSYSTEMTIME end,
    /* [in] */ DWORD flags,
    /* [in] */ DWORD cRequested,
    /* [out] */ DWORD *pcRuntimes,
    /* [size_is][size_is][out] */ PSYSTEMTIME *pRuntimes);

HRESULT SchRpcGetLastRunInfo( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [out] */ PSYSTEMTIME pLastRuntime,
    /* [out] */ DWORD *pLastReturnCode);

HRESULT SchRpcGetTaskInfo( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [in] */ DWORD flags,
    /* [out] */ DWORD *pEnabled,
    /* [out] */ DWORD *pState);

HRESULT SchRpcGetNumberOfMissedRuns( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [out] */ DWORD *pNumberOfMissedRuns);

HRESULT SchRpcEnableTask( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ const wchar_t *path,
    /* [in] */ DWORD enabled);



extern RPC_IF_HANDLE ITaskSchedulerService_v1_0_c_ifspec;
extern RPC_IF_HANDLE ITaskSchedulerService_v1_0_s_ifspec;
#endif /* __ITaskSchedulerService_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


