#include "ScheduleTask.h"

BOOL PigScheduleTask::Initialization() {

	//  Initialize COM.
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    if (FAILED(hr))
    {
        printf("\nCoInitializeEx failed: %x", hr);
        return 1;
    }

    //  Set general COM security levels.
    hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);

    if (FAILED(hr))
    {
        printf("\nCoInitializeSecurity failed: %x", hr);
        CoUninitialize();
        return 1;
    }

    //  Create an instance of the Task Service. 
    hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);

    if (FAILED(hr))
    {
        printf("Failed to create an instance of ITaskService: %x", hr);
        CoUninitialize();
        return 1;
    }

    //  Connect to the task service.
    hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());

    if (FAILED(hr))
    {
        printf("ITaskService::Connect failed: %x", hr);
        pService->Release();
        CoUninitialize();
        return 1;
    }

    //  Get the pointer to the root task folder.  This folder will hold the
    //  new task that is registered.
    hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
    if (FAILED(hr))
    {
        printf("Cannot get Root folder pointer: %x", hr);
        pService->Release();
        CoUninitialize();
        return 1;
    }

    return 0;
}

BOOL PigScheduleTask::CreateTask(char* wszTaskName, char* wszProgramPath, char* wszProgramArgs, char* wszAuthor, char* wszDescription) {

    //  If the same task exists, remove it.
    pRootFolder->DeleteTask(_bstr_t(wszTaskName), 0);

    //  Create the task definition object to create the task.
    hr = pService->NewTask(0, &pTask);

    pService->Release();  // COM clean up.  Pointer is no longer used.
    if (FAILED(hr))
    {
        printf("Failed to CoCreate an instance of the TaskService class: %x", hr);
        pRootFolder->Release();
        CoUninitialize();
        return 1;
    }

    //  Get the registration info for setting the identification.
    hr = pTask->get_RegistrationInfo(&pRegInfo);
    if (FAILED(hr))
    {
        printf("\nCannot get identification pointer: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //covert BSTR
    //BSTR bstrAuthor = _com_util::ConvertStringToBSTR(wszAuthor);
    //BSTR bstrDescription = _com_util::ConvertStringToBSTR(wszDescription);

    hr = pRegInfo->put_Author(_bstr_t(wszAuthor));
    hr = pRegInfo->put_Description(_bstr_t(wszDescription));
    pRegInfo->Release();

    if (FAILED(hr))
    {
        printf("\nCannot put identification info: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Create the principal for the task - these credentials
    //  are overwritten with the credentials passed to RegisterTaskDefinition
    hr = pTask->get_Principal(&pPrincipal);
    if (FAILED(hr))
    {
        printf("\nCannot get principal pointer: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Set up principal logon type to interactive logon
    hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);

    // Set up the Run Level **low priv
    hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_LUA);
    pPrincipal->Release();
    if (FAILED(hr))
    {
        printf("\nCannot put principal info: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Create the settings for the task
    hr = pTask->get_Settings(&pSettings);
    if (FAILED(hr))
    {
        printf("\nCannot get settings pointer: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Set setting values for the task.  
    hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
    pSettings->Release();
    if (FAILED(hr))
    {
        printf("\nCannot put setting information: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    // Set the idle settings for the task.
    hr = pSettings->get_IdleSettings(&pIdleSettings);
    if (FAILED(hr))
    {
        printf("\nCannot get idle setting information: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }


    hr = pTask->get_Triggers(&pTriggerCollection);
    if (FAILED(hr))
    {
        printf("\nCannot get trigger collection: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    ////  Add the LOGON trigger to the task.
    //hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
    //pTriggerCollection->Release();

    //if (FAILED(hr))
    //{
    //    printf("\nCannot create trigger: %x", hr);
    //    pRootFolder->Release();
    //    pTask->Release();
    //    CoUninitialize();
    //    return 1;
    //}

    
    //代码可以参考：https://github.com/DavidGinzberg/ScheduleIt/blob/master/ScheduleIt/ScheduleIt.cpp
    
    //Time Trigger
     hr = pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
    pTriggerCollection->Release();

    if (FAILED(hr))
    {
        printf("\nCannot create trigger: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    ITimeTrigger* pTimeTrigger = NULL;
    hr = pTrigger->QueryInterface(
        IID_ITimeTrigger, (void**)&pTimeTrigger);
    pTrigger->Release();
    if (FAILED(hr))
    {
        printf("\nQueryInterface call failed for ITimeTrigger: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    hr = pTimeTrigger->put_Id(_bstr_t(L"Trigger1"));
    if (FAILED(hr))
        printf("\nCannot put trigger ID: %x", hr);

    IRepetitionPattern* pRepetitionPattern = NULL;
    hr = pTimeTrigger->get_Repetition(&pRepetitionPattern);
    if (FAILED(hr)) {
        printf("\nFailed to get repetition pattern: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }
    //run every 20 minutes
    pRepetitionPattern->put_Interval(_bstr_t(L"PT10M"));
    hr = pTimeTrigger->put_Repetition(pRepetitionPattern);
    if (FAILED(hr)) {
        printf("\nFailed to put repetition pattern: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    hr = pTimeTrigger->put_StartBoundary(_bstr_t(L"2005-01-01T12:05:00"));
    pTimeTrigger->Release();
    if (FAILED(hr))
    {
        printf("\nCannot add start boundary to trigger: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }
    

    //  Add an action to the task.   
    IActionCollection* pActionCollection = NULL;
    //  Get the task action collection pointer.
    hr = pTask->get_Actions(&pActionCollection);
    if (FAILED(hr))
    {
        printf("\nCannot get Task collection pointer: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Create the action, specifying that it is an executable action.
    hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
    pActionCollection->Release();
    if (FAILED(hr))
    {
        printf("\nCannot create the action: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  QI for the executable task pointer.
    hr = pAction->QueryInterface(
        IID_IExecAction, (void**)&pExecAction);
    pAction->Release();
    if (FAILED(hr))
    {
        printf("\nQueryInterface call failed for IExecAction: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Set the path of the executable
    hr = pExecAction->put_Path(_bstr_t(wszProgramPath));
    // args
    hr = pExecAction->put_Arguments(_bstr_t(wszProgramArgs));

    pExecAction->Release();
    if (FAILED(hr))
    {
        printf("\nCannot put action path: %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //char xml[] = "<?xml version=\"1.0\" ?>"
    //    "< !--"
    //    "This sample schedules a task to start notepad.exe when a user logs on."
    //    "-- >"
    //    "<Task xmlns = \"http://schemas.microsoft.com/windows/2004/02/mit/task\">"
    //    "<RegistrationInfo>"
    //    "< Date>2005 - 10 - 11T13:21 : 17 - 08 : 00 < / Date >"
    //    "<Author>AuthorName< / Author>"
    //    "< Version>1.0.0 < / Version >"
    //    "<Description>Starts Notepad when a specified user logs on.< / Description>"
    //    "< / RegistrationInfo>"
    //    "<Triggers>"
    //    "<LogonTrigger>"
    //    "< StartBoundary>2005 - 10 - 11T13:21 : 17 - 08 : 00 < / StartBoundary >"
    //    "< EndBoundary>2006 - 01 - 01T00 : 00 : 00 - 08 : 00 < / EndBoundary >"
    //    "<Enabled>true< / Enabled>"
    //    "<UserId>DOMAIN_NAME\\UserName< / UserId>"
    //    "< / LogonTrigger>"
    //    "< / Triggers>"
    //    "<Principals>"
    //    "<Principal>"
    //    "<GroupId>Builtin\Administrators< / GroupId>"
    //    "< / Principal>"
    //    "< / Principals>"
    //    "<Settings>"
    //    "<Enabled>true< / Enabled>"
    //    "<AllowStartOnDemand>true< / AllowStartOnDemand>"
    //    "<AllowHardTerminate>true< / AllowHardTerminate>"
    //    "< / Settings>"
    //    "<Actions>"
    //    "<Exec>"
    //    "<Command>notepad.exe< / Command>"
    //    "< / Exec>"
    //    "< / Actions>"
    //    "< / Task>";
    //  Save the task in the root folder.
    //IRegisteredTask* pRegisteredTask = NULL;
    hr = pRootFolder->RegisterTaskDefinition(_bstr_t(wszTaskName), pTask, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pRegisteredTask);
    //hr = pRootFolder->RegisterTask(_bstr_t("test"), _bstr_t(xml), TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pRegisteredTask);
    if (FAILED(hr))
    {
        printf("\nError saving the Task : %x", hr);
        pRootFolder->Release();
        pTask->Release();
        CoUninitialize();
        return 1;
    }

    //  Clean up.
    pRootFolder->Release();
    pTask->Release();
    pRegisteredTask->Release();
    CoUninitialize();
    return 0;
}

BOOL PigScheduleTask::DeleteTask(char* TaskName) {

    hr = pRootFolder->DeleteTask(_bstr_t(TaskName), 0);
    if (FAILED(hr))
    {
        return FALSE;
    }

    return 0;
}