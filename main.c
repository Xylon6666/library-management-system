#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "library.h"

int main()
{
	//获取控制台标准输出句柄
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	//判断是否是首次运行
	int runTimes = isFirstRun();
	if (runTimes == -1)
	{
		showError("判断首次运行失败，程序退出！");
		return -1;
	}
	//执行对应初始化逻辑
	int runResult = 0;
	if (runTimes == 1)
	{
		//首次运行：执行初始化
		runResult = firstRun();
	}
	else 
	{
		//非首次：加载+检查
		runResult = normalRun();
	}
	// 初始化失败则退出
	if (runResult != 1)
	{
		showError("初始化失败，程序退出！");
		return -1;
	}

	clearScreen();

	while(1)
	{
		clearScreen();
		//登录
		if (login() < 0)
		{
			return -1;
		}
		//读者
		if (g_currentUserSign == 0)
		{
			personalCenter();
		}
		//普通管理员
		else if (g_currentUserSign == 1)
		{
			normalAdminMenu();
		}
		//超级管理员
		else if (g_currentUserSign == 2)
		{
			superAdminMenu();
		}
	}
	return 0;
}