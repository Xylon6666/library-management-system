#pragma once
#include "common.h"

/********************* 数据持久化模块（data.c实现） *********************/
int fileExists(const char* filePath);    //检查文件是否存在
int isFirstRun();    //判断是否是首次运行
int firstRun();    //首次运行初始化（仅第一次执行）
int normalRun();    //非首次运行：加载数据+检查文件
void modifyGlobalVars();    //修改全局变量
int saveGlobalVars();    // 保存全局变量到data.dat

void* creatNode(size_t nodeSize);    //创建链表节点
void freeBookList(Book* head);    //释放图书链表
int loadBooks();    // 从文件读取图书数据到内存
int saveBooks(Book* head);    // 把内存图书数据写入文件

void freeUserList(User* head);    //释放用户链表
int loadUsers();    // 读取用户数据
int saveUsers(User* head);    // 保存用户数据

void freeBorrowList(Borrow* head);    // 释放借阅链表
int loadBorrows();       // 读取借阅文件到内存
int saveBorrows(Borrow* head);    // 将借阅数据写入文件

void saveListToCsv(int choice);    //保存数据为csv文件

/********************* 数据统计模块（statistics.c实现） *********************/
void countBooks();    //有关图书的各种统计
void maxBorrowedTime();    //借阅次数前10的书籍
void maxFineUsers();    //罚金前5的用户

/********************* 身份验证模块（auth.c实现） *********************/
void getPassword(char* pwd);	//安全输入密码（隐藏输入）
int login();    //登录
void modifyPassword();    //修改密码

/********************* 菜单模块（menu.c实现） *********************/
void centerPrint(int width, const char* str);    //居中显示字符串
void setColor(int color);    //设置文字颜色
void loginMenu();          //登录界面菜单
void mainMenu();   // 主菜单标题部分
void normalAdminMenu();       //普通管理员菜单
void superAdminMenu();       //超级管理员菜单
void personalCenter();   //个人中心菜单
void borrowManagementMenu();    //借阅管理菜单
void bookManagementMenu();    //图书管理菜单
void readerManagementMenu();    //读者管理菜单
void dataExportMenu();    //信息导出菜单
void statisticsMenu();    //数据统计菜单
void borrowRecordMenu();    //借阅记录菜单

/********************* 个人中心模块（personal.c实现） *********************/
void myBorrow();   // 查询我的借阅记录
void fineStatistics();    //罚金统计
int checkBorrowOverdueStatus();    //借阅预警提示
void renewBook();    //续借图书

/********************* 核心业务模块（business.c实现） *********************/
//辅助函数
int countGbkDot(const char* str);    //统计字符串里的·
int isLeapYear(int year);    //判断是否为闰年
int getDaysOfMonth(int year, int month);    //获取每个月的天数
long dateToDays(int year, int month, int day);    //计算从0年0月0日到现在的天数
void daysToDate(long totalDays, int* year, int* month, int* day);    //将从0年0月0日到现在的天数转换成日期
int parseDateString(const char* dateStr, int* year, int* month, int* day);    //解析字符串为日期
int getCurrentDateStr(char* dateStr);    //获取当前日期并转换为字符串
int countUserBorrowedBooks(char* userId);    //统计读者未归还借阅数
int maskIdCard(char* src, char* dst);    //遮挡身份证号（保留前6位+后4位，中间8位用*代替）

// 图书管理
void addBook();    //添加图书（管理员）
void delBook();    //删除图书（管理员）
void searchBook();    //查询图书（所有人）
void modifyBook();    //修改图书信息（管理员）
// 读者管理（管理员）
void addUser();    //添加用户
void searchUser();    //查找用户
void delUser();    //删除用户
// 借阅归还
void borrowBook();    //借阅图书
int isBookOverDue(const char* borrowDate, int* intervalDay);    //判断是否逾期
void returnBook();    //归还图书
void modifyReturnDate();    //修改归还时间