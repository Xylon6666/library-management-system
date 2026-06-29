#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "library.h"

/*************************************************
  Function:居中输出字符串
  Description:按照给定长度居中输出字符串
  Calls:setColor
  Called By:loginMenu,mainMenu,addBook,addUser,borrowBook
           ,delBook,delUser,fineStatistics,modifyBook
           ,myBorrow,returnBook,searchBook,searchUser
  Input:int width固定长度，char* str需要显示的字符串
  Output:无
  Return:无
  Others:格式:*****>> str <<*****
*************************************************/
void centerPrint(int width, const char* str) 
{
    //设置颜色
    setColor(0x0E); //黑底亮黄
    //获取字符串实际长度
    int strLen = (int)strlen(str);
    //最小需要的长度：字符串长度 + 左右各1个空格
    int minRequired = strLen + 6;
    //边界处理：无法满足左右空格要求时，直接输出原字符串
    if (minRequired > width) 
    {
        printf("%s", str);
        return;
    }
    //计算左右需要填充的*号数量
    int totalStars = width - minRequired;
    int leftStars = totalStars / 2;
    int rightStars = totalStars - leftStars;
    //打印
    for (int i = 0; i < leftStars; i++) 
    {
        printf("*");
    }
    printf(">> %s <<", str);
    for (int i = 0; i < rightStars; i++) 
    {
        printf("*");
    }
    printf("\n");
    setColor(0x0F);
}

//登录菜单
void loginMenu()
{
    printf("┌──────────────────────────────────┐\n");
    printf("│      ");
    //设置颜色
    setColor(0x0E); //黑底亮黄
    printf("欢迎使用图书管理系统！");
    setColor(15);
    printf("      │\n");
    printf("└──────────────────────────────────┘\n");
    setColor(3);
    printf(" 提示：系统所有输入操作均以回车确定\n");
    setColor(15);
    centerPrint(36, "请先登录");
}

//主菜单
void mainMenu()
{
    centerPrint(46, g_currentUserName);
    //如果是读者，显示即将到期书籍数
    if(g_currentUserSign == 0)
    {
        checkBorrowOverdueStatus();
    }
    //每一类用户的菜单头部标题
	printf("┌────────────────────────────────────────────┐\n");
    printf("│                                            │\n");
    printf("│                ");
    setColor(14);
    printf("图书管理系统");
    setColor(15);
    printf("                │\n"); 
    printf("│                                            │\n");
    printf("├────────────────────────────────────────────┤\n");
}

//普通管理员子菜单
void normalAdminMenu()
{
    while(1)
    {
        //打印主菜单标题部分
        mainMenu();
        printf("│                 1.书籍查询                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 2.借阅管理                 │\n");
        printf("│                 3.图书管理                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 4.用户管理                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 5.数据统计                 │\n");
        printf("│                 6.信息导出                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 7.修改密码                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 ");
        setColor(79);
        printf("0.退出登录");
        setColor(15);
        printf("                 │\n");
        printf("└────────────────────────────────────────────┘\n");
        printf("请选择功能：>");
        //接收并判断运行
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            searchBook();
        }
        else if (choice == 2)
        {
            clearScreen();
            borrowManagementMenu();
        }
        else if (choice == 3)
        {
            clearScreen();
            bookManagementMenu();
        }
        else if (choice == 4)
        {
            clearScreen();
            readerManagementMenu();
        }
        else if (choice == 5)
        {
            clearScreen();
            statisticsMenu();
        }
        else if (choice == 6)
        {
            clearScreen();
            dataExportMenu();
        }
        else if (choice == 7)
        {
            clearScreen();
            modifyPassword();
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
}

//超级管理员子菜单
void superAdminMenu()
{
    while (1)
    {
        //打印主菜单标题部分
        mainMenu();
        printf("│                 1.书籍查询                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 2.借阅管理                 │\n");
        printf("│                 3.图书管理                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 4.用户管理                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 5.数据统计                 │\n");
        printf("│                 6.信息导出                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 7.修改设定                 │\n");
        printf("│                 8.修改密码                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 ");
        setColor(79);
        printf("0.退出登录");
        setColor(15);
        printf("                 │\n");
        printf("└────────────────────────────────────────────┘\n");
        printf("请选择功能：>");
        //接收并判断运行
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            searchBook();
        }
        else if (choice == 2)
        {
            clearScreen();
            borrowManagementMenu();
        }
        else if (choice == 3)
        {
            clearScreen();
            bookManagementMenu();
        }
        else if (choice == 4)
        {
            clearScreen();
            readerManagementMenu();
        }
        else if (choice == 5)
        {
            clearScreen();
            statisticsMenu();
        }
        else if (choice == 6)
        {
            clearScreen();
            dataExportMenu();
        }
        else if (choice == 7)
        {
            clearScreen();
            modifyGlobalVars();
        }
        else if (choice == 8)
        {
            clearScreen();
            modifyPassword();
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
}

// 个人中心 
void personalCenter()
{
    while (1)
    {
        //打印主菜单标题部分
        mainMenu();
        printf("│                 1.书籍查询                 │\n");
        printf("│                 2.借阅中心                 │\n");
        printf("│                 3.罚金查询                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 4.修改密码                 │\n");
        printf("├────────────────────────────────────────────┤\n");
        printf("│                 ");
        setColor(79);
        printf("0.退出登录");
        setColor(15);
        printf("                 │\n");
        printf("└────────────────────────────────────────────┘\n");
        printf("请选择功能：>");
        //接收并判断运行
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            searchBook();
        }
        else if (choice == 2)
        {
            clearScreen();
            borrowRecordMenu();
        }
        else if (choice == 3)
        {
            clearScreen();
            fineStatistics();
        }
        else if (choice == 4)
        {
            clearScreen();
            modifyPassword();
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
}

//借阅菜单
void borrowManagementMenu()
{
    while(1)
    {
        printf("┌──────────────────────────────────────┐\n");
        printf("│              ");
        setColor(14);
        printf("借阅管理");
        setColor(15);
        printf("                │\n");
        printf("├──────────────────────────────────────┤\n");
        printf("│            1.借阅书籍                │\n");
        printf("│            2.归还书籍                │\n");
        if (g_currentUserSign == 2)
        {
            printf("│            3.批量修改借阅日期        │\n");
        }
        printf("│            ");
        setColor(79);
        printf("0.返回上级菜单");
        setColor(15);
        printf("            │\n");
        printf("└──────────────────────────────────────┘\n");
        printf("请选择功能：>");
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            borrowBook();
        }
        else if (choice == 2)
        {
            clearScreen();
            returnBook();
        }
        else if (choice == 3 && g_currentUserSign == 2)
        {
            clearScreen();
            modifyReturnDate();
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
    return;
}

//图书管理
void bookManagementMenu()
{
    while (1)
    {
        printf("┌──────────────────────────────────┐\n");
        printf("│             ");
        setColor(14);
        printf("图书管理");
        setColor(15);
        printf("             │\n");
        printf("├──────────────────────────────────┤\n");
        printf("│            1.采编入库            │\n");
        printf("│            2.清除库存            │\n");
        printf("│            3.信息维护            │\n");
        printf("│            ");
        setColor(79);
        printf("0.返回上级菜单");
        setColor(15);
        printf("        │\n");
        printf("└──────────────────────────────────┘\n");
        printf("请选择功能：>");
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            addBook();
        }
        else if (choice == 2)
        {
            clearScreen();
            delBook();
        }
        else if (choice == 3)
        {
            clearScreen();
            modifyBook();
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
    return;
}

//用户管理
void readerManagementMenu()
{
    while (1)
    {
        printf("┌──────────────────────────────────┐\n");
        printf("│            ");
        setColor(14);
        printf("用户管理");
        setColor(15);
        printf("              │\n");
        printf("├──────────────────────────────────┤\n");
        printf("│            1.查询                │\n");
        printf("│            2.新增                │\n");
        printf("│            3.删除                │\n");
        printf("│            ");
        setColor(79);
        printf("0.返回上级菜单");
        setColor(15);
        printf("        │\n");
        printf("└──────────────────────────────────┘\n");
        printf("请选择功能：>");
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            searchUser();
        }
        else if (choice == 2)
        {
            clearScreen();
            addUser();
        }
        else if (choice == 3)
        {
            clearScreen();
            delUser();
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
    return;
}

//信息导出
void dataExportMenu()
{
    while (1)
    {
        printf("┌──────────────────────────────────┐\n");
        printf("│             ");
        setColor(14);
        printf("信息导出");
        setColor(15);
        printf("             │\n");
        printf("├──────────────────────────────────┤\n");
        printf("│            1.图书数据            │\n");
        printf("│            2.借阅数据            │\n");
        printf("│            ");
        setColor(79);
        printf("0.返回上级菜单");
        setColor(15);
        printf("        │\n");
        printf("└──────────────────────────────────┘\n");
        printf("请选择功能：>");
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            saveListToCsv(1);
        }
        else if (choice == 2)
        {
            clearScreen();
            saveListToCsv(2);
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
    return;
}

//数据统计
void statisticsMenu()
{
    while (1)
    {
        printf("┌──────────────────────────────────┐\n");
        printf("│             ");
        setColor(14);
        printf("数据统计");
        setColor(15);
        printf("             │\n");
        printf("├──────────────────────────────────┤\n");
        printf("│          1.图书数据统计          │\n");
        printf("│          2.借阅书籍排行榜        │\n");
        printf("│          3.罚金用户排行榜        │\n");
        printf("│          ");
        setColor(79);
        printf("0.返回上级菜单");
        setColor(15);
        printf("          │\n");
        printf("└──────────────────────────────────┘\n");
        printf("请选择功能：>");
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            countBooks();
        }
        else if (choice == 2)
        {
            clearScreen();
            maxBorrowedTime();
        }
        else if (choice == 3)
        {
            clearScreen();
            maxFineUsers();
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
    return;
}

void borrowRecordMenu()
{ 
    while (1)
    { 
        printf("┌──────────────────────────────────────┐\n");
        printf("│               ");
        setColor(14);
        printf("借阅中心");
        setColor(15);
        printf("               │\n");
        printf("├──────────────────────────────────────┤\n");
        printf("│            1.续借图书                │\n");
        printf("│            2.未归还图书信息          │\n");
        printf("│            3.导出全部借阅记录        │\n");
        printf("│            ");
        setColor(79);
        printf("0.返回上级菜单");
        setColor(15);
        printf("            │\n");
        printf("└──────────────────────────────────────┘\n");
        printf("请选择功能：>");
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
            renewBook();
        }
        else if (choice == 2)
        {
            clearScreen();
            myBorrow();
        }
        else if (choice == 3)
        {
            clearScreen();
            saveListToCsv(3);
        }
        else if (choice == 0)
        {
            break;
        }
        else
        {
            showError("输入错误,请重新输入！");
            Sleep(1000);
        }
        clearScreen();
    }
}