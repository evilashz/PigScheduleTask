#pragma once
#define _WIN32_DCOM
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <comdef.h>
#include <wincred.h>
//  Include the task header file.
#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")

using namespace std;

class PigScheduleTask {

public:
	BOOL Initialization();
	BOOL CreateTask(char* wszTaskName, char* wszProgramPath, char* wszProgramArgs, char* wszAuthor, char* wszDescription);
	BOOL DeleteTask(char* TaskName);

private:
	//Initialization
	HRESULT hr;
	ITaskService* pService = NULL;
	ITaskFolder* pRootFolder = NULL;

	//CreateTask
	ITaskDefinition* pTask = NULL;
	IRegistrationInfo* pRegInfo = NULL;
	IPrincipal* pPrincipal = NULL;
	ITaskSettings* pSettings = NULL;
	IIdleSettings* pIdleSettings = NULL;
	ITriggerCollection* pTriggerCollection = NULL;
	ITrigger* pTrigger = NULL;
	IAction* pAction = NULL;
	IActionCollection* pActionCollection = NULL;
	IExecAction* pExecAction = NULL;
	IRegisteredTask* pRegisteredTask = NULL;

};