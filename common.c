#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "library.h"

 /*全局变量定义*/
HANDLE hOut;    //控制台输出句柄
char g_currentUserName[NAME_LEN] = "";    //当前用户名
char g_currentUserId[UID_LEN] = "";    //当前用户名
int g_currentUserSign = -3;    //当前用户身份标识
Book* g_bookHead = NULL;    //图书链表头指针
User* g_userHead = NULL;    //用户链表头指针
Borrow* g_borrowHead = NULL;    //借阅信息链表头指针
int g_tryMaxTimes = DEFAULT_g_tryMaxTimes;    //login函数中登录尝试最多次数
int g_maxBorrowDays = DEFAULT_g_maxBorrowDays;    //书籍的最大借阅时间
int g_singleDayFine = DEFAULT_g_singleDayFine;  //一本书的违约金
int g_maxBorrowQuality = DEFAULT_g_maxBorrowQuality;  //最多借阅数量
int g_renewDays = DEFAULT_g_renewDays;    //续借天数

/************************ 清屏函数实现 ************************/
void clearScreen() 
{
    system("cls");
}

/************************ 暂停函数实现 ************************/
void pauseScreen(const char* tip) 
{
    int ch;
    if(_kbhit())//检测缓冲区是否存在东西，如果有就清空
    {
        while ((ch = getchar()) != '\n' && ch != EOF);
    }
    setColor(243);//设置控制台颜色为白底 + 深青色字
    printf("\n%s", tip);
    setColor(0x0F);//还原控制台颜色
    (void)_getch();//等待键盘按下
}

/************************ 设置控制台颜色 ************************/
void setColor(int color) 
{
    SetConsoleTextAttribute(hOut, color);
}

/************************ 获取数字 ************************/
int getNumber() 
{
    int num;//存储最终合法的整数
    int scanResult;//存储scanf的返回值，用于判断输入是否有效
    char ch;//用于清空输入缓冲区的临时变量
    while (1) 
    {
        //printf("请输入一个整数：");
        scanResult = scanf("%d", &num);
        //如果读取到了数字
        if (scanResult == 1) 
        {
            //清空缓冲区中剩余的换行符（避免影响后续输入）
            while ((ch = getchar()) != '\n' && ch != EOF);
            return num;
        }
        //没有读取到数字
        else 
        {
            //清空输入缓冲区中所有错误的字符（直到换行符）
            while ((ch = getchar()) != '\n' && ch != EOF);
            //提示用户重新输入
            printf("输入错误！请输入有效的数字。\n");
        }
    }
}

/************************ 字符串转数字 ************************/
int strToNumber(const char* str)
{
    int num = 0;
    while (*str != '\0')
    {
        num = num * 10 + (*str - '0');
        str++;
    }
    return num;
}

/************************ 展示错误信息 ************************/
void showError(const char* str)
{
    setColor(12);//设置控制台颜色为亮红
    printf("%s\n", str);
    setColor(15);//还原控制台颜色
}

/************************ 展示成功信息 ************************/
void showSuccess(const char* str)
{
    setColor(10);//设置控制台颜色为亮绿
    printf("%s\n", str);
    setColor(15);//还原控制台颜色
}