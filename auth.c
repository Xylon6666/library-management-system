#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "library.h"
/*************************************************
  Function:getPassword
  Description:适配PWD_LEN=15的密码输入函数（无栈溢出）
  Calls:无
  Called By:login
  Input:char* pwd - 存储密码的数组（长度必须≥PWD_LEN）
  Output:屏幕显示*，支持退格删除、回车确认
  Return:无
  Others:1. 最多输入14个有效字符（适配PWD_LEN=15）；
         2. 严格限制越界写入，彻底避免栈溢出；
         3. 仅接收可见字符（ASCII 32~126）。
*************************************************/
void getPassword(char* pwd)
{
    int i = 0;// 密码字符计数（0~14）
    char ch = 0;// 临时捕获输入字符

    //强制初始化密码数组为0（清空残留数据）
    memset(pwd, 0, PWD_LEN);

    while (1)
    {
        ch = _getch();// 无回显捕获键盘输入

        //回车确认：结束输入
        if (ch == '\r' || ch == '\n')
        {
            if (i < PWD_LEN) 
            {
                //确保不越界
                pwd[i] = '\0';
            }
            else 
            {
                pwd[PWD_LEN - 1] = '\0';//极端情况强制补结束符
            }
            printf("\n");//回车换行
            break;
        }

        //退格键：删除最后一个字符（仅i>0时操作）
        else if (ch == 8 && i > 0)
        {
            i--;// 计数回退
            pwd[i] = '\0';// 清空当前位置字符
            printf("\b \b");// 屏幕删除一个*（回退+空格+回退）
        }

        //仅接收非空格的可见字符（ASCII 33~126）
        else if (ch >= 33 && ch <= 126 && i < (PWD_LEN - 1))
        {
            pwd[i] = ch;//写入字符（0~13位）
            i++;//计数+1（最大到14）
            printf("*");//屏幕显示*
        }
    }
}

/*************************************************
  Function:登录函数
  Description:登录验证函数
  Calls:loadUsers(),loginMenu();
  Called By:      // 调用本函数的函数清单
  Input:无
  Output:无
  Return:2-超级管理员，1-普通管理员，0-读者，
         -1-读取用户列表失败，-2-多次验证失败
  Others:多次尝试登录失败后返回-2
*************************************************/
int login()
{
    //获取用户链表
    User* head = NULL;
    User* p = NULL;
    //获取成功
    if (loadUsers())
    {
        head = g_userHead;
    }
    //获取失败
    else
    {
        return -1;
    }
    //获取用户输入的信息
    char userId[UID_LEN] = { 0 };
    char pwd[PWD_LEN] = { 0 };
    int tryTimes = 0;
    //循环输入部分直到输入正确 或 到达最大尝试次数
    while(1)
    {
        //输出登录标题
        loginMenu();
        //累加登录次数
        tryTimes++;
        //接收用户输入的信息
        printf("请输入用户ID：>");
        (void)scanf("%8s", userId);
        // 清空scanf残留的换行符（避免密码输入直接结束）
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        printf("请输入密码：>");
        getPassword(pwd);
        //验证
        p = head;
        while (p != NULL)
        {
            if (strcmp(userId, p->userId) == 0)//匹配用户ID
            {
                break;
            }
            p = p->next;
        }
        //如果尝试次数小于设置值
        if (tryTimes < g_tryMaxTimes)
        {
            if ((p != NULL) && (strcmp(pwd, p->pwd) == 0))//判断密码是否匹配
            {
                break;
            }
            else
            {
                setColor(4);
                printf("用户ID或密码错误！\n");
                setColor(0x0F);

                printf("\n剩余尝试次数：%d", 3 - tryTimes);
                pauseScreen("\n按下任意键继续…");
            }
        }
        else
        {
            setColor(4);
            printf("尝试次数达到限制！\n");
            setColor(0x0F);
            return -2;
        }
        clearScreen();
    }
    //设置全局变量值
    strncpy(g_currentUserName, p->userName, NAME_LEN - 1);
    g_currentUserName[NAME_LEN - 1] = '\0'; // 强制补结束符
    strncpy(g_currentUserId, p->userId, UID_LEN - 1);
    g_currentUserId[UID_LEN - 1] = '\0';    // 强制补结束符
    g_currentUserSign = p->isAdmin;

    //输出当前用户的名称
    setColor(10);
    printf("\n用户>>%s<<登录成功!\n", p->userName);
    setColor(15);
    pauseScreen("按下任意键进入主菜单…");
    clearScreen();
    //返回当前用户身份
    return p->isAdmin;
}

/*************************************************
  Function:modifyPassword
  Description:修改密码
  Calls:centerPrint,clearScreen,loadUsers,pauseScreen
       ,saveUsers,setColor,showError,showSuccess
  Called By:main
  Input:无
  Output:当前用户数据
  Return:无
  Others:密码6-14位，可以有空格
*************************************************/
void modifyPassword()
{
    //密码
    char oldPwd[PWD_LEN] = "";
    //打印标题
    centerPrint(32,"修改密码");
    //读取用户信息
    User* head = NULL;
    User* p = NULL;
    if (loadUsers())
    {
        head = g_userHead;
    }
    else
    {
        return;
    }
    //遍历查找到当前用户
    p = head;
    while (p != NULL)
    {
        if (strcmp(g_currentUserId, p->userId) == 0)
        {
            break;
        }
        p = p->next;
    }
    //查找到之后
    if (p != NULL)
    {
        //直到输入正确原密码
        while(1)
        {
            clearScreen();
            //打印标题
            centerPrint(32, "修改密码");
            printf("请输入原密码：>");
            getPassword(oldPwd);
            if (strcmp(oldPwd, p->pwd) == 0)
            {
                break;
            }
            else
            {
                pauseScreen("\n原密码错误，按任意键重试！");
                memset(oldPwd, 0, PWD_LEN); //清空错误密码
            }
        }
        char newPwd1[PWD_LEN] = "";
        //直到两次新密码输入正确
        while(1)
        {
            //清屏+打印标题
            clearScreen();
            centerPrint(42, "修改密码");
            setColor(3);//青色
            printf("注意：密码需为6-14位！\n");
            setColor(15);
            memset(newPwd1, 0, PWD_LEN); //每次循环清空，避免残留
            printf("请输入新密码：>");
            getPassword(newPwd1);

            if (strcmp(newPwd1, oldPwd) == 0)
            {
                pauseScreen("\n原密码不能和旧密码相同，按任意键重试！");
                continue;
            }
            else if (strlen(newPwd1)<6)
			{
                pauseScreen("\n密码必须大于等于6位，按任意键重试！");
                continue;
			}

            char newPwd2[PWD_LEN] = "";
            printf("请再次输入新密码：>");
            getPassword(newPwd2);

            if (strcmp(newPwd1, newPwd2) == 0)
            {
                break;
            }
            else
            {
                pauseScreen("\n两次输入的密码不一致，按任意键重试！");
                memset(newPwd1, 0, PWD_LEN);
                memset(newPwd2, 0, PWD_LEN);
            }
        }
        strcpy(p->pwd, newPwd1);
    }

    // 校验保存结果：仅保存成功才提示修改成功
    if (saveUsers(head))
    {
        showSuccess("\n密码修改成功，请牢记你的新密码!");
    }
    else
    {
        showError("\n密码修改失败！保存用户数据出错，请重试！");
        // 回滚密码：避免内存改了但文件没保存
        if(p != NULL)
        {
            strncpy(p->pwd, oldPwd, PWD_LEN - 1);
            p->pwd[PWD_LEN - 1] = '\0';
        }
    }

    pauseScreen("按下任意键返回主菜单…");
}