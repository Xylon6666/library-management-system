#pragma once
/*公共头文件*/
#include <stdio.h>
#include <stdlib.h> //动态内存管理
#include <string.h> //字符串相关内容
#include <windows.h>//控制台
#include <errno.h>  //打印错误码
#include <conio.h>  //_kbhit和_getch(检测缓冲区是否有字符、获取字符)
#include <time.h>   //获取当前时间
#include <fcntl.h>  //提供 _O_TEXT 宏(以ANSI格式写入文件用)
#include <io.h>     //提供 fileno() 和 _setmode() 函数声明(判断文件是否存在和设置写入格式)

/*公共常量*/
#define FILE_BOOK "books.txt"  //图书数据文件
#define FILE_USER "users.dat"  //用户数据文件
#define FILE_BORROW "borrow.txt"  //借阅数据文件
#define FILE_DATA "data.dat"    //程序运行所需数据文件
#define PWD_LEN 15       //密码最大长度
#define NAME_LEN 50      //图书/用户名最大长度
#define IDCARD_LEN 19   //身份证号长度（18位+1位'\0'）
#define AUTHOR_LEN 33      //作者最大长度
#define ISBN_LEN 14      //ISBN固定长度
#define UID_LEN 9      //USERID固定长度
#define DATE_LEN 20      //日期固定长度
#define MAX_PATH 260    //导出时用户输入的路径长度
#define DEFAULT_g_tryMaxTimes 3   //默认-login函数中登录尝试最多次数
#define DEFAULT_g_maxBorrowDays 30    //默认-书籍的最大借阅时间
#define DEFAULT_g_singleDayFine 1   //默认-一本书的违约金
#define DEFAULT_g_maxBorrowQuality 5   //默认-最多借阅数量
#define DEFAULT_g_renewDays 7    //默认-续借天数

/*全局结构体*/
//图书结构体
typedef struct BOOK
{
    char ISBN[ISBN_LEN];     // 图书编号（唯一）
    char bookName[NAME_LEN]; // 图书名
    char author[AUTHOR_LEN];   // 作者
    char publisher[NAME_LEN];// 出版社
    int inventory;           // 库存数量
    int borrowedCount;       // 已借出数量
    int borrowTimes;         // 累计借阅次数
    struct BOOK* next;
}Book;

// 用户结构体
typedef struct USER
{
    char userId[UID_LEN];    // 用户ID
    char pwd[PWD_LEN];       // 密码
    char userName[NAME_LEN]; // 用户名
    char idCard[IDCARD_LEN]; //身份证号（18位，支持最后一位X）
    int isAdmin;             // 身份标识：2-超级管理员，1-普通管理员，0-读者
    struct USER* next;
}User;

// 借阅记录结构体
typedef struct BORROW 
{
    char userId[UID_LEN];     // 读者信息：用户ID
    char ISBN[ISBN_LEN];      // 书籍信息：图书ISBN
    char borrowDate[DATE_LEN];// 借阅日期：格式YYYY-MM-DD
    char returnDate[DATE_LEN];// 归还日期：格式YYYY-MM-DD
    int isRenew;              // 续借状态：0=未续借，1=已续借
    int isReturn;             // 归还状态：0=未归还，1=已归还
    struct BORROW* next;
}Borrow;

/*全局变量声明*/
extern char g_currentUserName[NAME_LEN];    //当前用户名
extern char g_currentUserId[UID_LEN];    //当前用户ID
extern int g_currentUserSign;    //当前用户身份标识
extern HANDLE hOut;    //控制台输出句柄
extern Book* g_bookHead;    //图书链表头指针
extern User* g_userHead;    //用户链表头指针
extern Borrow* g_borrowHead;    //借阅信息链表头指针
extern int g_tryMaxTimes;    //login函数中登录尝试最多次数
extern int g_maxBorrowDays;    //书籍的最大借阅时间
extern int g_singleDayFine;  //一本书的违约金
extern int g_maxBorrowQuality;  //最多借阅数量
extern int g_renewDays;    //续借天数

/*通用工具*/
void clearScreen();    // 清屏函数
void pauseScreen(const char* tip);    // 暂停屏幕（按任意键继续）
int getNumber();    //获取数字
int strToNumber(const char* temp);    //字符串转数字
void showError(const char* str);    //展示错误信息
void showSuccess(const char* str);    //展示成功信息