#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "library.h"

/*************************************************
  Function:MyBorrow
  Description: 查询并展示当前登录用户的未归还图书借阅记录
  Calls:centerPrint(),loadBooks(),loadBorrows(),freeBookList(),freeBorrowList()
  Called By:main
  Input:无显式输入参数，依赖全局变量 g_currentUserId（当前登录用户ID）
  Output:控制台输出借阅记录列表（未归还图书）、无记录/异常提示信息
  Return:无
  Others:1. 若借阅链表/图书链表加载失败，直接提示异常并返回；
         2. 仅展示 isReturn 为 false 的未归还借阅记录；
         3. 输出格式为左对齐，ISBN占20字符、书名50字符、借阅日期10字符。
*************************************************/
void myBorrow()
{
    //输出功能标题
    centerPrint(78,"未归还图书查询");
    //读取借阅和图书信息
    Borrow* borrowhead = NULL;
    Book* bookhead = NULL;
    //读取成功
    if (loadBorrows() && loadBooks())
    {
        borrowhead = g_borrowHead;
        bookhead = g_bookHead;
    }
    //读取失败
    else
    {
        return;
    }
    Borrow* p = borrowhead;
    //输出表头
    printf("┌──────────────┬──────────────────────────────────────────────────┬──────────┐\n");
    printf("│%-14s│%-50s│%-10s│\n","ISBN编号","书名","借阅日期");
    printf("├──────────────┼──────────────────────────────────────────────────┼──────────┤\n");
    int has_record = 0;//是否输出一条有效数据
    //遍历链表
    while (p != NULL)
    {
        //如果图书没有归还 并且 是当前用户借阅的
        if ((!p->isReturn) && strcmp(p->userId, g_currentUserId) == 0)
        {
			int interval = 0;//接收借阅天数
            if(isBookOverDue(p->returnDate,&interval))
            {
                setColor(12);
			}
            Book* q = bookhead;
            has_record = 1;
            //查找当前借阅数据对应的书籍
            while (q != NULL)
            {
                if (strcmp(q->ISBN, p->ISBN) == 0)
                    break;
                q = q->next;
            }
            if (q != NULL)
            {
                printf("│%-14s│%-50s", q->ISBN, q->bookName);
                //补书名中·造成的打印空格
                int booknameDot = countGbkDot(q->bookName);
                for (int i = 0; i < booknameDot; i++)
                {
                    printf(" ");
                }
                printf("│%-10s│\n", p->borrowDate);
                setColor(15);
                printf("├──────────────┼──────────────────────────────────────────────────┼──────────┤\n");
            }
        }
        p = p->next;
    }
    //如果没有输出借阅数据
    if (!has_record)
    {
        printf("│%-14s│%-50s│%-10s│\n", "-", "暂无未归还的图书", "-");
        printf("└──────────────┴──────────────────────────────────────────────────┴──────────┘\n");
    }
    //有输出的借阅数据
    else
    {
        printf("└──────────────┴──────────────────────────────────────────────────┴──────────┘\n");
    }
    printf("注：逾期图书将以红色显示\n");
    //释放链表
    freeBookList(bookhead);
    freeBorrowList(borrowhead);
    pauseScreen("按任意键返回主菜单…");
    return;
}

/*************************************************
  Function:fineStatistics
  Description:罚金统计
  Calls:loadBooks,loadBorrows,centerPrint,centerPrint
       ,isBookOverDue,pauseScreen,setColor
  Called By:main
  Input:无
  Output:显示逾期图书信息和罚金总数
  Return:无
  Others:无
*************************************************/
void fineStatistics()
{
    //输出功能标题
    centerPrint(93, "罚金统计");
    //读取图书和借阅数据
    Book* bookhead = NULL;
    Borrow* borrowhead = NULL;
    //如果读取成功
    if (loadBooks() && loadBorrows())
    {
        bookhead = g_bookHead;
        borrowhead = g_borrowHead;
    }
    //读取失败
    else
    {
        return;
    }
    //打印表头
    printf("┌───────────────────────────────────────────────────────────────────────────────────────────┐\n");
	printf("│                                       逾期图书列表                                        │\n");
    printf("├────────────────────┬──────────────────────────────────────────────────┬──────────┬────────┤\n");
    printf("│%-20s│%-50s│%10s│%-8s│\n",
        "ISBN", "书名", "借阅时间 ", "逾期天数");
    printf("├────────────────────┼──────────────────────────────────────────────────┼──────────┼────────┤\n");
    Borrow* borrowp = borrowhead;
    int totalFine = 0;//是否输出逾期借阅数据
    //遍历链表
    while (borrowp != NULL)
    {
        //查找当前用户的 未归还 的书籍
        if (strcmp(borrowp->userId, g_currentUserId) == 0 && borrowp->isReturn == 0)
        {
            int interval = 0;//接收借阅天数
            //判断是否逾期
            if (isBookOverDue(borrowp->borrowDate,&interval))
            {
                Book* bookp = bookhead;
                //查找当前逾期书籍的信息
                while (bookp != NULL)
                {
                    if (strcmp(bookp->ISBN, borrowp->ISBN) == 0)
                    {
                        break;
                    }
                    bookp = bookp->next;
                }
				if (bookp != NULL)
                {
                    printf("│%-20s│%-50s", bookp->ISBN, bookp->bookName);
                    //补空格解决书名中的·造成的打印错位
                    int booknameDot = countGbkDot(bookp->bookName);
                    for (int i = 0; i < booknameDot; i++)
                    {
                        printf(" ");
                    }
                    printf("│%-10s│%8d│\n", borrowp->borrowDate, interval);
                    printf("├────────────────────┼──────────────────────────────────────────────────┼──────────┼────────┤\n");
                    totalFine += (interval*g_singleDayFine);//统计罚金
                }
            }
        }
        borrowp = borrowp->next;
    }
    //如果没有逾期书籍
    if (totalFine == 0)
    {
        printf("│%-20s│%-50s│%-10s│%-8s│\n", "-", "没有逾期图书", "-", "-");
        printf("└────────────────────┴──────────────────────────────────────────────────┴──────────┴────────┘\n");
    }
    else
    {
        printf("└────────────────────┴──────────────────────────────────────────────────┴──────────┴────────┘\n");
    }
    //如果有罚金则提示单本罚金
    if(totalFine > 0)
    {
        printf("(单日罚金%d元)\n", g_singleDayFine);
        printf("(当前最长借阅天数为：%d天)\n", g_maxBorrowDays);
    }
    //输出罚金
    setColor(6);
    printf("罚金总计：%d元\n", totalFine);
    setColor(15);
    pauseScreen("按任意键返回主菜单…");
    return;
}

/*************************************************
  Function: checkBorrowOverdueStatus
  Description: 统计当前登录读者未归还图书：已逾期/即将逾期，分两行提示
  Calls:getCurrentDateStr,parseDateString,dateToDays
  Called By:mainMenu
  Input:无（使用全局变量 g_currentUserId）
  Output:打印逾期、即将逾期提示
  Return:总预警数量，-1=异常
  Others:
*************************************************/
int checkBorrowOverdueStatus()
{
    //加载借阅记录链表
    if (!loadBorrows())
    {
        showError("加载借阅记录失败！");
        return -1;
    }
    Borrow* p = g_borrowHead;
    if (p == NULL)
    {
        return 0;
    }

    //获取当前日期并转换为总天数
    char currentDate[DATE_LEN] = "";
    if (!getCurrentDateStr(currentDate))
    {
        showError("获取系统时间失败！");
        return -1;
    }
    int currentYear, currentMonth, currentDay;
    parseDateString(currentDate, &currentYear, &currentMonth, &currentDay);
    long currentDays = dateToDays(currentYear, currentMonth, currentDay);

    int warnCount = 0;    //即将逾期（剩余0~3天）
    int overdueCount = 0; //已逾期（剩余天数<0）

    // 遍历所有借阅记录
    while (p != NULL)
    {
        //筛选条件：当前登录用户 + 未归还图书
        if (strcmp(p->userId, g_currentUserId) == 0 && p->isReturn == 0)
        {
            int expireYear, expireMonth, expireDay;
            //解析应还日期
            if (parseDateString(p->returnDate, &expireYear, &expireMonth, &expireDay))
            {
                long expireDays = dateToDays(expireYear, expireMonth, expireDay);
                long remainDays = expireDays - currentDays;

                //已逾期：剩余天数 < 0
                if (remainDays < 0)
                {
                    overdueCount++;
                }
                //即将逾期：剩余天数 0~3天
                else if (remainDays >= 0 && remainDays <= 3)
                {
                    warnCount++;
                }
            }
        }
        p = p->next;
    }

    //分两行打印提示（颜色区分）
    //已逾期提示（红色）
    if (overdueCount > 0)
    {
        setColor(12);
        printf("【逾期警告】您有 %d本 图书已经逾期，请尽快归还！\n", overdueCount);
        setColor(15);
    }
    //即将逾期提示（黄色）
    if (warnCount > 0)
    {
        setColor(11);
        printf("【借阅预警】您有 %d本 图书即将逾期，请及时归还！\n", warnCount);
        setColor(15);
    }

    // 返回总预警数量
    return warnCount + overdueCount;
}

/*************************************************
  Function: renewBook
  Description: 读者续借图书，仅限1次，按全局设定的固定天数续借
  Calls: loadBorrows, parseDateString, dateToDays, daysToDate, saveBorrows
  Called By: 读者功能菜单
  Input: 无
  Output: 续借结果、新应还日期
  Return: 无
  Others: 依赖全局变量 g_renewDays / g_currentUserId
*************************************************/
void renewBook()
{
    //加载借阅数据
    if (!loadBorrows())
    {
        showError("加载借阅记录失败！");
        pauseScreen("按任意键继续…");
        return;
    }
    Borrow* head = g_borrowHead;

    centerPrint(38, "图书续借");
    char targetISBN[ISBN_LEN] = "";
    printf("请输入要续借的图书ISBN：>");
    (void)scanf("%13s", targetISBN);
    //清空输入缓冲区
    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF);

    //查找可续借的记录
    Borrow* p = head;
    while (p != NULL)
    {
        // 筛选条件：本人借阅 + 未归还 + 未续借过
        if (strcmp(p->userId, g_currentUserId) == 0 &&
            strcmp(p->ISBN, targetISBN) == 0 &&
            p->isReturn == 0 &&
            p->isRenew == 0)
        {
            break;
        }
        p = p->next;
    }

    //无符合条件的图书
    if (p == NULL)
    {
        showError("续借失败！原因：未借阅/已归还/已完成1次续借");
        pauseScreen("按任意键返回上级菜单…");
        return;
    }

    //自动计算新应还日期（使用全局固定续借天数）
    int oldY, oldM, oldD;
    parseDateString(p->returnDate, &oldY, &oldM, &oldD);
    long oldDays = dateToDays(oldY, oldM, oldD);
    long newDays = oldDays + g_renewDays;

    //转换为日期格式
    int newY, newM, newD;
    daysToDate(newDays, &newY, &newM, &newD);
    //更新应还日期
    sprintf(p->returnDate, "%04d-%02d-%02d", newY, newM, newD);
    //标记已续借（永久限制1次）
    p->isRenew = 1;

    //保存数据并提示
    setColor(10);
    printf("\n续借成功！\n");
    printf("系统设定续借天数：%d天\n", g_renewDays);
    printf("新应还日期：%s\n", p->returnDate);
    setColor(15);
    saveBorrows(head);

    pauseScreen("按任意键继续…");
}