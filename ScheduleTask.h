#pragma once
#include "header.h"

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