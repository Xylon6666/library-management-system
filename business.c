#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "library.h"
 
/*************************************************
  Function:countGbkDot
  Description:统计ANSI(GBK)编码字符串中全角间隔号“·”的个数；
              该字符在GBK编码格式下为双字节组合0xA1 0xA4，函数通过匹配该双字节
              精准识别目标字符；
  Calls:无
  Called By:searchBook,modifyBook,myBorrow,fineStatistics
  Input:str - 输入的待统计字符串（书名/作者名等），GBK编码格式；
  Output:无
  Return:int类型非负整数，返回字符串中“·”的总个数；
         特殊返回值：输入为空指针/空字符串时，返回0。
  Others:1. 仅适配GBK编码的“·”，若字符串为UTF-8编码，
            需修改字节匹配值（UTF-8的“·”为0xC2 0xB7）；
         2. 匹配到双字节字符后会跳过第二个字节，避免重复统计；
         3. 函数对输入字符串为只读操作（const修饰），不会修改原字符串内容。
*************************************************/
int countGbkDot(const char* str) 
{
    //空字符串直接返回0
    if (str == NULL || *str == '\0') 
    {
        return 0;
    }
    int dotNumer = 0;
    //遍历字符串，匹配GBK的·（0xA1 0xA4双字节）
    for (int i = 0; str[i] != '\0'; i++) 
    {
        if ((unsigned char)str[i] == 0xA1 && (unsigned char)str[i + 1] == 0xA4) 
        {
            dotNumer++;
            i++; // 跳过第二个字节（双字节字符）
        }
    }
    return dotNumer;
}

/*************************************************
  Function:isLeapYear
  Description:判断是否为闰年
  Calls:无
  Called By:getDaysOfMonth,dateToDays
  Input:year-需要判断的年份
  Output:无
  Return:1-是闰年 0-不是闰年
  Others:无
*************************************************/
int isLeapYear(int year)
{
    //闰年规则：能被4整除但不能被100整除，或能被400整除
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/*************************************************
  Function:getDaysOfMonth
  Description:获取每个月的天数
  Called By:dateToDays,parseDateString
  Input:year-年份 month-月份
  Output:无
  Return:对应月份的天数
  Others:考虑闰年
*************************************************/
int getDaysOfMonth(int year, int month)
{
    //不符合月份返回0
    if (month < 1 || month>12)
        return 0;
    int monthDays[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
    //闰年2月特殊处理
    if (month == 2 && isLeapYear(year))
    {
        return 29;
    }
    return monthDays[month];
}

/*************************************************
  Function:dateToDays
  Description:将年/月/日转换为总天数
  Calls:isLeapYear,getDaysOfMonth
  Called By:isBookOverDue
  Input:year,month,day
  Output:无
  Return:从0年0月0日到现在的总天数
  Others:基准：0年0月0日
*************************************************/
long dateToDays(int year, int month, int day)
{
    long totalDays = 0;
    //累加年的总天数
    for (int y = 0; y < year; y++)
    {
        //闰年特殊处理
        totalDays += (isLeapYear(y) ? 366 : 365);
    }
    //累加月的总天数
    for (int m = 0; m < month; m++)
    {
        totalDays += getDaysOfMonth(year, m);
    }
    //累加日的总天数
    totalDays += day;
    return totalDays;
}

/*************************************************
  Function:daysToDate
  Description:将总天数转换为年/月/日
  Calls:isLeapYear,getDaysOfMonth
  Input:totalDays-总天数,year/month/day-输出参数指针
  Output:转换后的年月日
  Return:无
  Others:无
*************************************************/
void daysToDate(long totalDays, int* year, int* month, int* day)
{
    int y = 0;
    //计算年份：逐年减去天数，确定年份
    while (1)
    {
        int yearDays = isLeapYear(y) ? 366 : 365;
        if (totalDays > yearDays)
        {
            totalDays -= yearDays;
            y++;
        }
        else
        {
            break;
        }
    }
    int m = 1;
    //计算月份：逐月减去天数，确定月份
    while (1)
    {
        int monthDays = getDaysOfMonth(y, m);
        if (totalDays > monthDays)
        {
            totalDays -= monthDays;
            m++;
        }
        else
        {
            break;
        }
    }
    //剩余天数就是日期
    *year = y;
    *month = m;
    *day = (int)totalDays;
}

/*************************************************
  Function:parseDateString
  Description:解析YYYY-MM-DD格式字符串为年/月/日
  Calls:getDaysOfMonth
  Called By:isBookOverDue
  Input:dateStr-需要解析的字符串地址,year-年地址,month-月地址,day-日地址
  Output:无
  Return:1-成功 0-失败
  Others:Unix时间戳的 “起始点”为1970年，所以年需大于1970
*************************************************/
int parseDateString(const char* dateStr, int* year, int* month, int* day)
{
    //检查字符串长度（YYYY-MM-DD共10个字符）
    if (strlen(dateStr) != 10) 
    {
        showError("日期字符串格式错误：长度必须为10");
        return 0;
    }
    // 格式解析
    int ret = sscanf(dateStr, "%d-%d-%d", year, month, day);
    if (ret != 3) 
    {
        showError("日期字符串格式错误：必须为YYYY-MM-DD");
        return 0;
    }
    //合法性检查
    if (*year < 1970 || *month < 1 || *month > 12 || *day < 1) 
    {
        showError("日期数值不合法（年<1970/月/日越界）");
        return 0;
    }
    //检查日期是否符合当月天数
    int maxDay = getDaysOfMonth(*year, *month);
    if (*day > maxDay) 
    {
        setColor(12);
        printf("日期数值不合法：%d年%d月没有%d天", *year, *month, *day);
        setColor(15);
        return 0;
    }

    return 1;
}

/*************************************************
  Function:getCurrentDateStr
  Description:获取当前日期并格式化为YYYY-MM-DD字符串
  Calls:localtime,showError,sprintf,time
  Called By:borrowBook,isBookOverDue,returnBook
  Input:dateStr-接收当前时间的字符串地址
  Output:当前时间的字符串
  Return:1=成功，0=失败
  Others:         // 其它说明
*************************************************/
int getCurrentDateStr(char* dateStr)
{
    //获取系统时间戳
    /* time_t 是C标准库中表示时间戳的类型（本质是长整型）
       ，存储从 1970-01-01 00:00:00 UTC 到当前时刻的秒数；
    */
    //time(NULL) 调用time函数获取当前系统时间戳，参数NULL表示不需要将结果存入额外的指针变量。
    time_t now = time(NULL);
    if (now == (time_t)-1) 
    {
        showError("获取系统时间失败");
        pauseScreen("按任意键继续…");
        return 0;
    }
    //转换为本地时间
    //struct tm 是C标准库的时间结构体，包含年、月、日、时、分、秒等字段；
    //localtime(&now) 将time_t类型的时间戳转换为本地时区的tm结构体指针
    struct tm* localTime = localtime(&now);
    if (localTime == NULL) 
    {
        showError("转换本地时间失败");
        pauseScreen("按任意键继续…");
        return 0;
    }
    // 格式化：YYYY-MM-DD（tm_year是1900开始，tm_mon是0开始）
    sprintf(dateStr, "%04d-%02d-%02d",
        localTime->tm_year + 1900,
        localTime->tm_mon + 1,
        localTime->tm_mday);
    return 1;
}

/*************************************************
  Function:addBook
  Description:添加图书
  Calls:loadBooks,centerPrint,getNumber,clearSceen
       ,saveBooks,creatNode,setColor,showSuccess
  Called By:bookManagementMenu() 
  Input:无
  Output:当前图书信息为数据文件
  Return:无
  Others:无
*************************************************/
void addBook()
{
    //读取当前图书数据
    Book* head = NULL;
    Book* tail = NULL;
    if (loadBooks())
    {
        head = g_bookHead;
    }
    else
    {
        return;
    }
    //是否添加标志
    int isAdd = 0;
    //循环添加图书
    while(1)
    {
        //打印标题
        centerPrint(30, "添加图书");
        //接收ISBN编号
        char isbn[ISBN_LEN] = "";
        int isContinue = 0;
        Book* p = head;
        printf("请输入ISBN编号：>");
        (void)scanf("%20s", isbn);
        // 清空输入缓冲区（处理残留字符）
        while (getchar() != '\n');
        //遍历链表查找是否存在图书
        while (p != NULL)
        {
            if (strcmp(isbn, p->ISBN) == 0)
            {
                break;
            }
            p = p->next;
        }
        //如果为已存在图书
        if (p != NULL)
        {
            char bookname[NAME_LEN] = "";
            int num = 0;
            setColor(11);//黑底亮青
            printf("此为已入库图书《%s》\n", p->bookName);
            setColor(15);
            printf("请输入本次添加数量： > ");
            (void)scanf("%d",&num);
            printf("是否确认添加？1-是 其余数字-否\n");
            int choice = getNumber();
            if(choice == 1)
            {
                p->inventory += num;
                isAdd = 1;
                showSuccess("添加成功！");
            }
            else
            {
                showSuccess("添加操作已取消！");
            }
        }
        //为不存在的图书
        else
        {
            tail = head;
            //若链表不为空，则遍历到链表尾节点
            while (tail != NULL && tail->next != NULL)
            {
                tail = tail->next;
            }
            //尝试开辟新内存
            Book* newbook = (Book*)creatNode(sizeof(Book));
            //如果开辟成功
            if (newbook != NULL)
            {
                strcpy(newbook->ISBN, isbn);
                printf("请输入书名：>");
                (void)scanf("%50s", newbook->bookName); 
                // 清空输入缓冲区（处理残留字符）
                while (getchar() != '\n');
                printf("请输入作者：>");
                (void)scanf("%33s", newbook->author);
                // 清空输入缓冲区（处理残留字符）
                while (getchar() != '\n'); 
                printf("请输入出版社：>");
                (void)scanf("%50s", newbook->publisher);
                // 清空输入缓冲区（处理残留字符）
                while (getchar() != '\n');
                printf("请输入数量：>");
                int number = 0;
                (void)scanf("%d", &number);
                newbook->inventory = number;
                newbook->borrowedCount = 0;
                newbook->borrowTimes  = 0;
                newbook->next = NULL;
                printf("\n是否确认添加？1-是 其余数字-否\n");
                int choice = getNumber();
				if (choice == 1)
                {
                    //链表不为空
                    if (tail != NULL)
                    {
                        tail->next = newbook;
                    }
                    //链表为空
                    else
                    {
                        tail = newbook;
                    }
                    isAdd = 1;
                    showSuccess("添加成功！");
                }
                else
                {
                    free(newbook);
                    showSuccess("添加操作已取消！");
                }
            }
        }
        printf("\n是否继续添加？1-是 其余数字-否\n");
        isContinue = getNumber();
        if (isContinue == 1)
        {
            clearScreen();
        }
        else
        {
            break;
        }
    }
    //如果成功添加，保存当前图书数据
    if(isAdd == 1)
    {
        (void)saveBooks(head);
    }
}

/*************************************************
  Function:delBook
  Description:删除图书
  Calls:loadBooks,centerPrint,clearScreen,saveBooks
       ,getNumber,setColor,showError,showSuccess 
  Called By:bookManagementMenu
  Input:无
  Output:当前图书数据
  Return:无
  Others:无
*************************************************/
void delBook()
{
    //读取当前图书数据
    Book* bookhead = NULL;
    Borrow* borrowHead = NULL;
    if (loadBooks() && loadBorrows())
    {
        bookhead = g_bookHead;
        borrowHead = g_borrowHead;
    }
    else
    {
        return;
    }
    //如果读取成功
    //如果链表头指针为空，即没有图书
    if (bookhead == NULL)
    {
        centerPrint(28, "清除库存");
        showError("当前没有在库图书！");
        pauseScreen("按任意键返回…");
        return;
    }
    else if(bookhead != NULL)
    {
        //循环删除图书
        while(1)
        {
            centerPrint(38, "清除库存");
            //接收输入的ISBN编号
            char isbn[ISBN_LEN] = "";
            printf("请输入ISBN编号：>");
            (void)scanf("%20s", isbn);
            // 清空输入缓冲区（处理残留字符）
            while (getchar() != '\n');
            //匹配节点
            Book* p = bookhead;
            Book* prev = NULL;
            while (p != NULL) 
            {
                //找到目标节点，退出循环
                if (strcmp(p->ISBN, isbn) == 0) 
                {
                    break; 
                }
                prev = p;    //先更新前驱节点
                p = p->next; //再移动当前节点
            }
            //查找到相关图书
            if (p != NULL)
            {
                //输出书籍信息
                setColor(11);
                printf("对应查找到的图书信息为：\n");
                printf("  ISBN：%-20s\n  书名：%-50s\n  作者：%-33s\n出版社：%-50s\n  数量：%d\n"
                    , p->ISBN, p->bookName, p->author, p->publisher, p->inventory);
                setColor(15);
                printf("\n是否确认删除？1-是 其余数字-否\n");
                int isDel = getNumber();
                //确认删除
                if (isDel == 1)
                {
                    int hasUnreturnBook = 0;
                    Borrow* borrow = borrowHead;
                    while (borrow != NULL)
                    {
                        if (strcmp(p->ISBN, borrow->ISBN) == 0 && borrow->isReturn == 0)
                        {
                            hasUnreturnBook = 1;
                            break;
                        }
                        borrow = borrow->next;
                    }
                    if (hasUnreturnBook == 0)
                    {
                        if (prev == NULL)//前驱指针为空代表删除的是头节点
                        {
                            bookhead = p->next;//直接修改头指针
                        }
                        else
                        {
                            prev->next = p->next;
                        }
                        free(p);
                        p = NULL;
                        showSuccess("删除成功！\n");
                    }
                    else
                    {
                        showError("此图书存在未归还的借阅记录，无法删除！");
                    }
                }
                //不确认删除则不进行任何操作
            }
            //未查找到图书
            else
            {
                showError("没有查找到图书，请检查ISBN编号是否正确！");
            }
            printf("\n是否继续删除图书？1-是 其余数字-否\n");
            int isContinue = getNumber();
            if (isContinue == 1)
            {
                clearScreen();
            }
            else
            {
                break;
            }
        }
    }
    (void)saveBooks(bookhead);
}

/*************************************************
  Function:searchBook
  Description:按关键词查找图书
  Calls:count_gbk_dit,loadBooks,getNumber
       ,centerPrint,clearScreen,showError
  Called By:main
  Input:无
  Output:无
  Return:无
  Others:无
*************************************************/
void searchBook()
{
    //读取图书数据
    Book* head = NULL;
    if (loadBooks())
    {
        head = g_bookHead;
    }
    else
    {
        return;
    }
    //判断头指针状态
    //如果头指针为空，即为没有图书
    if (head == NULL)
    {
        centerPrint(28, "图书查找");
        showError("当前没有图书！");
        pauseScreen("按任意键返回…");
        return;
    }
    else if(head != NULL)
    {
        //循环输出菜单
        while(1)
        {
            //打印标题
            centerPrint(42, "图书查找");
            int has_book = 0;
            Book* p = head;
            printf("请选择查找条件：1-ISBN编号    2-书名关键字\n                3-作者关键字  4-出版社\n                0-返回上级菜单\n");
            int choice = getNumber();
            char key_word[50] = "";
            clearScreen();
            //按ISBN查找
            if (choice == 1)
            {
                centerPrint(44, "按ISBN查找");
                int has_book = 0;
                printf("请输入ISBN编号(准确查找)：>");
                (void)scanf("%13s", key_word);
                // 清空输入缓冲区（处理残留字符）
                while (getchar() != '\n');
                printf("┌──────────────┬──────────────────────────────────────────────────┬─────────────────────────────────┬──────────────────────────────────────────────────┬────────┐\n");
                printf("│%-14s│%-50s│%-33s│%-50s│%-8s│\n",
                    "ISBN", "书名", "作者", "出版社", "库存数量");
                printf("├──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
                while (p != NULL)
                {
                    if (strcmp(p->ISBN, key_word) == 0)
                    {
                        has_book = 1;
                        //调用封装的函数统计·的个数
                        int booknameDot = countGbkDot(p->bookName);
                        int authorDot = countGbkDot(p->author);
                        int totalDot = booknameDot + authorDot;
                        //打印内容+补空格
                        printf("│%-14s│%-50s│%-33s", p->ISBN, p->bookName, p->author);
                        for (int i = 0; i < totalDot; i++)
                        {
                            printf(" ");
                        }
                        printf("│%-50s│%8d│\n", p->publisher, p->inventory);
                        printf("├──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
                    }
                    p = p->next;
                }
                if (has_book)
                {
                    printf("└──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");
                }
                else
                {
                    printf("│%-14s│%-50s│%-33s│%-50s│%-8s│\n",
                        "-", "没有匹配的图书", "-", "-", "-");
                    printf("└──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");
                }
            }
            //按书名查找
            else if (choice == 2)
            {
                centerPrint(44, "按书名查找");
                printf("请输入关键字（最多 24个汉字 或 49个英文字母）\n");
                (void)scanf("%49s", key_word);
                // 清空输入缓冲区（处理残留字符）
                while (getchar() != '\n');
                printf("┌──────────────┬──────────────────────────────────────────────────┬─────────────────────────────────┬──────────────────────────────────────────────────┬────────┐\n");
                printf("│%-14s│%-50s│%-33s│%-50s│%-8s│\n",
                    "ISBN", "书名", "作者", "出版社", "库存数量");
                printf("├──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
                while (p != NULL)
                {
                    if (strstr(p->bookName, key_word) != NULL)
                    {
                        has_book = 1;
                        //调用封装的函数统计·的个数
                        int booknameDot = countGbkDot(p->bookName);
                        int authorDot = countGbkDot(p->author);
                        int totalDot = booknameDot + authorDot;
                        //打印内容+补空格
                        printf("│%-14s│%-50s│%-33s", p->ISBN, p->bookName, p->author);
                        for (int i = 0; i < totalDot; i++)
                        {
                            printf(" ");
                        }
                        printf("│%-50s│%8d│\n", p->publisher, p->inventory);
                        printf("├──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
                    }
                    p = p->next;
                }
                if (has_book)
                {
                    printf("└──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");
                }
                else
                {
                    printf("│%-14s│%-50s│%-33s│%-50s│%-8s│\n",
                        "-", "没有匹配的图书", "-", "-", "-");
                    printf("└──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");
                }
            }
            //按作者查找
            else if (choice == 3)
            {
                //打印标题
                centerPrint(44, "按作者查找");
                int has_book = 0;
                printf("请输入关键字（最多 24个汉字 或 49个英文字母）\n");
                (void)scanf("%33s", key_word);
                // 清空输入缓冲区（处理残留字符）
                while (getchar() != '\n');
                printf("┌──────────────┬──────────────────────────────────────────────────┬─────────────────────────────────┬──────────────────────────────────────────────────┬────────┐\n");
                printf("│%-14s│%-50s│%-33s│%-50s│%-8s│\n",
                    "ISBN", "书名", "作者", "出版社", "库存数量");
                printf("├──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
                while (p != NULL)
                {
                    if (strstr(p->author, key_word) != NULL)
                    {
                        has_book = 1;
                        //调用封装的函数统计·的个数
                        int booknameDot = countGbkDot(p->bookName);
                        int authorDot = countGbkDot(p->author);
                        int totalDot = booknameDot + authorDot;
                        //打印内容+补空格
                        printf("│%-14s│%-50s│%-33s", p->ISBN, p->bookName, p->author);
                        for (int i = 0; i < totalDot; i++)
                        {
                            printf(" ");
                        }
                        printf("│%-50s│%8d│\n", p->publisher, p->inventory);
                        printf("├──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
                    }
                    p = p->next;
                }
                if (has_book)
                {
                    printf("└──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");
                }
                else
                {
                    printf("│%-14s│%-50s│%-33s│%-50s│%-8s│\n",
                        "-", "没有匹配的图书", "-", "-", "-");
                    printf("└──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");
                }
            }
            //按出版社查找
            else if (choice == 4)
            {
                //打印标题
                centerPrint(44, "按出版社查找");
                int has_book = 0;
                printf("请输入关键字（最多 24个汉字 或 49个英文字母）\n");
                (void)scanf("%49s", key_word);
                // 清空输入缓冲区（处理残留字符）
                while (getchar() != '\n');
                printf("┌──────────────┬──────────────────────────────────────────────────┬─────────────────────────────────┬──────────────────────────────────────────────────┬────────┐\n");
                printf("│%-14s│%-50s│%-33s│%-50s│%-8s│\n",
                    "ISBN", "书名", "作者", "出版社", "库存数量");
                printf("├──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
                while (p != NULL)
                {
                    if (strstr(p->publisher, key_word) != NULL)
                    {
                        has_book = 1;
                        //调用封装的函数统计·的个数
                        int booknameDot = countGbkDot(p->bookName);
                        int authorDot = countGbkDot(p->author);
                        int totalDot = booknameDot + authorDot;
                        //打印内容+补空格
                        printf("│%-14s│%-50s│%-33s", p->ISBN, p->bookName, p->author);
                        for (int i = 0; i < totalDot; i++)
                        {
                            printf(" ");
                        }
                        printf("│%-50s│%8d│\n", p->publisher, p->inventory);
                        printf("├──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
                    }
                    p = p->next;
                }
                if (has_book)
                {
                    printf("└──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");
                }
                else
                {
                    printf("│%-14s│%-50s│%-33s│%-50s│%-8s│\n",
                        "-", "没有匹配的图书", "-", "-", "-");
                    printf("└──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");
                }
            }
            else if (choice == 0)
            { 
                return;
            }
            else
            {
                showError("输入错误！");
            }
            printf("\n是否继续查找图书？1-是 其余数字-否\n");
            int isContinue = getNumber();
            if (isContinue == 1)
            {
                clearScreen();
            }
            else
            {
                break;
            }
        }
    }
}

/*************************************************
  Function:modifyBook
  Description:修改图书信息
  Calls:loadBooks,centerPrint,saveBooks,getNumber
       ,clearScreen,countGbkDot,mshowError,showSuccess 
  Called By:bookManagementMenu
  Input:无
  Output:修改过后的图书数据
  Return:无
  Others:无
*************************************************/
void modifyBook()
{
    //读取图书数据
    Book* head = NULL;
    if (loadBooks())
    {
        head = g_bookHead;
    }
    else
    {
        return;
    }
    //是否成功修改
    int isModify = 0;
    //判断头指针状态
    //头指针为空，则当前没有图书
    if (head == NULL)
    {
        centerPrint(28, "信息维护");
        showError("当前没有图书！");
        pauseScreen("按任意键返回…");
        return;
    }
    else if (head != NULL)
    {
        while (1)
		{
            //打印标题
			centerPrint(37, "信息维护");
            Book* p = head;
			//ISBN查找
			char isbn[ISBN_LEN] = "";
			printf("请输入ISBN编码：>");
			(void)scanf("%13s", isbn);
			// 清空输入缓冲区（处理残留字符）
			while (getchar() != '\n');
			//遍历查找
			while (p != NULL)
			{
				if (strcmp(isbn, p->ISBN) == 0)
				{
					break;//有匹配的则退出
				}
				p = p->next;
			}
			//找到 
			if (p != NULL)
			{
				printf("对应查找到的图书信息为：\n");
				printf("┌────────┬──────────────────────────────────────────────────┐\n");
				printf("│%-8s│%-50s│\n", "选项", "信息");
				printf("├────────┼──────────────────────────────────────────────────┤\n");
				printf("│%-8s│%-50s│\n", "1.ISBN", p->ISBN);
				printf("├────────┼──────────────────────────────────────────────────┤\n");
				printf("│%-8s│%-50s", "2.书名", p->bookName);
				//补点
				int nameDot = countGbkDot(p->bookName);
				for (int i = 0; i < nameDot; i++)
				{
					printf(" ");
				}
				printf("│\n");
				printf("├────────┼──────────────────────────────────────────────────┤\n");
				printf("│%-8s│%-50s", "3.作者", p->author);
				//补点
				int authorDot = countGbkDot(p->author);
				for (int i = 0; i < authorDot; i++)
				{
					printf(" ");
				}
				printf("│\n");
                printf("├────────┼──────────────────────────────────────────────────┤\n");
                printf("│%-8s│%-50s│\n", "4.出版社", p->publisher); 
                printf("├────────┼──────────────────────────────────────────────────┤\n");
                printf("│%-8s│%-50d│\n", "5.数量", p->inventory);
				printf("├────────┼──────────────────────────────────────────────────┤\n");
				printf("│%-8s│%-50s│\n", "其他数字", "退出");
				printf("└────────┴──────────────────────────────────────────────────┘\n");
				printf("请输入选项：>");
				int isChoice = getNumber();

                //临时存储原始值，用于取消修改时恢复
                char oldIsbn[ISBN_LEN] = "";
                char oldBookname[NAME_LEN] = "";
                char oldAuthor[AUTHOR_LEN] = "";
                char oldPublisher[NAME_LEN] = "";
                int oldInventory = p->inventory;
                strcpy(oldIsbn, p->ISBN);
                strcpy(oldBookname, p->bookName);
                strcpy(oldAuthor, p->author);
                strcpy(oldPublisher, p->publisher);
                //ISBN
				if (isChoice == 1)
				{
					char newIsbn[ISBN_LEN] = "";
					printf("请输入新ISBN编号：>");
					(void)scanf("%13s", newIsbn);
                    // 清空输入缓冲区（处理残留字符）
                    while (getchar() != '\n');
					strcpy(p->ISBN, newIsbn);
				}
                //书名
				else if (isChoice == 2)
				{
					char newBookname[NAME_LEN] = "";
					printf("请输入新书名：>");
					(void)scanf("%49s", newBookname);
                    // 清空输入缓冲区（处理残留字符）
                    while (getchar() != '\n');
					strcpy(p->bookName, newBookname);
				}
                //作者
                else if (isChoice == 3)
                {
                    char newAuthor[AUTHOR_LEN] = "";
                    printf("请输入新作者：>");
                    (void)scanf("%33s", newAuthor);
                    // 清空输入缓冲区（处理残留字符）
                    while (getchar() != '\n');
                    strcpy(p->author, newAuthor);
                }
                //出版社
                else if (isChoice == 4)
                {
                    char newPublisher[NAME_LEN] = "";
                    printf("请输入新出版社：>");
                    (void)scanf("%49s", newPublisher);
                    // 清空输入缓冲区（处理残留字符）
                    while (getchar() != '\n');
                    strcpy(p->author, newPublisher);
                }
                //数量
				else if (isChoice == 5)
				{
					int newInventory = 0;
                    while (1)
                    {
                        printf("请输入新数量：>");
                        (void)scanf("%d", &newInventory);
                        if (newInventory < 0)
                        {
                            showError("数量不能小于0，请重新输入！\n");
                            continue;
                        }
                        p->inventory = newInventory;
                        break;
                    }
				}
                //退出
				else
				{
                    showSuccess("已退出当前图书的修改\n");
				}
                //如果选择了修改
                if (isChoice >= 1 && isChoice <= 4)
                {
                    printf("\n是否确认修改？1-是 其余数字-否\n");
                    int confirm = getNumber();
                    if (confirm == 1)
                    {
                        isModify = 1;
                        showSuccess("图书信息修改成功！\n");
                    }
                    else
                    {
                        // 取消修改，恢复原始值
                        strcpy(p->ISBN, oldIsbn);
                        strcpy(p->bookName, oldBookname);
                        strcpy(p->author, oldAuthor);
                        strcpy(p->publisher, oldPublisher);
                        p->inventory = oldInventory;
                        showSuccess("修改操作已取消！\n");
                    }
                }
			}
			//未查找到图书
			else
			{
				showError("没有查找到图书，请检查ISBN编号是否正确！");
			}
            printf("\n是否继续维护图书信息？1-是 其他数字-否\n");
            int isContinue = getNumber();
            if (isContinue == 1)
            {
                clearScreen();
            }
            else
            {
                break;
            }
        }
    }
    //如果成功修改则保存当前图书数据
    if(isModify == 1)
    {
        (void)saveBooks(head);
    }
}

/*************************************************
  Function:addUser
  Description:添加用户
  Calls:loadUser,centerPrint,creatNode,setColor
       ,saveUsers,getNumber,setColor,showError,strToNumber
       ,showSuccess
  Called By:readerManagementMenu
  Input:无
  Output:当前用户信息
  Return:无
  Others:根据身份不同提供创建不同用户类型
*************************************************/
void addUser()
{
    //读取用户数据
    User* head = NULL;
    if (loadUsers())
    {
        head = g_userHead;
    }
    else
    {
        return;
    }
    //循环添加
    while (1)
    {
        //打印标题
        centerPrint(40, "添加用户");
        //新用户的标志-默认是读者0
        int newSign = 0;
        User* newUser = (User*)creatNode(sizeof(User));
        if (newUser != NULL)
        {
            User* p = head;
            //超级管理员可以添加普通管理员类型
            if (g_currentUserSign == 2)
            {
                printf("请选择添加用户类型：0-读者 1-普通管理员\n");
                newSign = getNumber();
                if (newSign > 1 || newSign < 0) 
                {
                    clearScreen();
                    free(newUser);
                    continue;
                }
            }
            newUser->isAdmin = newSign;

            printf("请输入用户名：>");
            (void)scanf("%49s", newUser->userName);
            //清空输入缓冲区（处理残留字符）
            while (getchar() != '\n');
			printf("请输入身份证号：>");
			(void)scanf("%19s", newUser->idCard);
            //处理身份证号末尾的X
            if (newUser->idCard[18] == 'x')
            {
                newUser->idCard[18] = 'X';
            }
            int isOnly = 1;
			//判断是否已经存在身份证号相同的用户
            while (p != NULL)
            {
                if (strcmp(newUser->idCard, p->idCard) == 0)
                {
                    isOnly = 0;
                    showError("已存在身份证号相同的用户，无法添加！");
                    break;
                }
                p = p->next;
			}
            //没有重复才能添加
            if(isOnly == 1)
            {
				//初始密码默认为123456
                strcpy(newUser->pwd,"123456");
                //生成新用户ID
                int isMiddle = 0;//连续排列中间有用户被删除之后新建时先利用之前的编号
                p = head;
				while (p != NULL && p->next != NULL)
                {
                    if (strToNumber(p->next->userId) != strToNumber(p->userId) + 1)
                    {
                        isMiddle = 1;
                        break;
                    } 
                    p = p->next;
                }
                int num = 0;
				if (p != NULL)
                {
                    num = strToNumber(p->userId);
                }
				//生成新用户ID，格式为8位数字，不足前面补0
                snprintf(newUser->userId, UID_LEN, "%08d", num + 1);

                //输出当前创建用户所有信息并确认  
                printf("\n当前新用户信息为：\n");
                printf("用户名：%s\n", newUser->userName);
				printf("身份证号：%s\n", newUser->idCard);
                setColor(96);//黄底黑字
                printf("用户ID：%s\n", newUser->userId);
                printf("初始密码：%s\n", newUser->pwd);
                setColor(15);
                printf("\n是否确认添加？1-是 其余数字-否\n");
                int confirm = getNumber();
                if (confirm == 1 && p!=NULL)
                {
                    //如果是中间插入
                    if (isMiddle)
                    {
                        User* temp = p->next;
                        p->next = newUser;
                        newUser->next = temp;
                    }
                    else
                    {
                        p->next = newUser;
                        newUser->next = NULL;
                    }
                    showSuccess("用户添加成功！");
                }
                else
                {
                    free(newUser);
                    showSuccess("已取消添加该用户！");
                }
            }
        }
        //是否继续添加
        printf("\n是否继续添加用户？1-是 其余数字-否\n");
        int isContinue = getNumber();
        if (isContinue == 1)
        {
            clearScreen();
        }
        else
        {
            break;
        }
    }
    (void)saveUsers(head);
}

/*************************************************
  Function:delUser
  Description:删除用户
  Calls:loadUser,centerPrint,getNumber,clearScreen
  Called By:readerManagementMenu
  Input:无
  Output:当前用户数据
  Return:无
  Others:无
*************************************************/
void delUser()
{
    //读取当前用户数据
    User* userhead = NULL;
    Borrow* borrowhead = NULL;
    if (loadUsers() && loadBorrows())
    {
        userhead = g_userHead;
        borrowhead = g_borrowHead;
    }
    else
    {
        return;
    }
    //是否进行了删除操作
    int isDelete = 0;
    //循环删除
    while (1)
    {
        //打印标题
        centerPrint(28, "删除用户");
        User* p = userhead;
        User* prev = userhead;
        Borrow* borrow = borrowhead;
        //接收输入的Id
		char id[UID_LEN] = "";
		printf("请输入用户ID：>");
		(void)scanf("%8s", id);
        // 清空输入缓冲区（处理残留字符）
        while (getchar() != '\n');
        //遍历链表查找用户
		while (p != NULL)
		{
            //普通管理员仅可查找读者
            if (g_currentUserSign == 1 && strcmp(id, p->userId) == 0 && p->isAdmin == 0)
            {
                setColor(11);
                printf("找到用户的信息为：\n");
                printf("用户名：%s\n用户ID：%s\n"
                    , p->userName, p->userId);
                setColor(15);
                break;
            }
            //超级管理员可以查找所有用户
            else if (g_currentUserSign == 2 && strcmp(id, p->userId) == 0 && (p->isAdmin != 2) )
            {
                setColor(11);
                printf("找到用户的信息为：\n");
                printf("用户名：%s\n用户ID：%s\n"
                    , p->userName, p->userId);
                setColor(15);
                break;
            }
            prev = p;
			p = p->next;
		}
        if (p != NULL)
        {
            int noBorrowed = 1;//是否存在未归还图书
            //遍历借阅记录查找当前用户是否存在未归还图书
            while (borrow != NULL)
            {
                if (strcmp(borrow->userId, id) == 0 && borrow->isReturn == 0)
                {
                    //存在标记为0
                    noBorrowed = 0;
                    break;
                }
                borrow = borrow->next;
            }
            //如果不存在未归还图书
            if(noBorrowed == 1)
            {
                printf("\n是否确认删除？1-是 其余数字-否\n");
                int comfirm = getNumber();
                if (comfirm == 1)
                {
                    prev->next = p->next;
                    free(p);
                    p = NULL;
                    isDelete = 1;//成功删除标记为1
                    showSuccess("删除成功！");
                }
            }
            //如果存在未归还图书
            else
            {
                showError("\n当前用户存在未归还图书，无法删除！");
            }
        }
		else if (p == NULL)
		{
            showError("未找到用户！");
		}
        //是否继续查找
        printf("\n是否继续删除用户？1-是 其余数字-否\n");
        int isContinue = getNumber();
        if (isContinue == 1)
        {
            clearScreen();
        }
        else
        {
            break;
        }
    }
    //存在成功删除操作再保存当前数据
    if(isDelete == 1)
    {
        (void)saveUsers(userhead);
    }
}

/*************************************************
  Function:searchUser
  Description:查找用户
  Calls:loadUsers(),centerPrint(),getNumber(),clearScreen(),showError
  Called By:readerManagementMenu
  Input:无
  Output:无
  Return:无
  Others:只能查找到用户的信息，不显示密码
*************************************************/
void searchUser()
{
    //读取用户数据
    User* head = NULL;
    if (loadUsers())
    {
        head = g_userHead;
    }
    else
    {
        return;
    }
    //循环查找
    while (1)
    {
        //打印标题
        centerPrint(33, "查找用户");
        User* p = head;
        printf("请选择查询类别：1-用户名 2-用户ID\n");
        int choice = getNumber();

        // 存储要显示的身份证号（完整/遮挡）
        char showIdCard[IDCARD_LEN] = "";

        //用户名
        if (choice == 1)
        {
            char name[NAME_LEN] = "";
            printf("请输入用户名：>");
            (void)scanf("%49s", name);
            // 清空输入缓冲区（处理残留字符）
            while (getchar() != '\n');

            // 标记是否找到用户
            int found = 0;
            printf("┌──────────────────────────────────────────────────┬────────┬───────────────────┐\n");
            printf("│%-50s│%-8s│%-19s│\n", "用户名", "用户ID", "身份证号");
            printf("├──────────────────────────────────────────────────┼────────┼───────────────────┤\n");
            while (p != NULL)
            {
                //普通管理员仅可查找读者
                if (g_currentUserSign == 1 && strcmp(name, p->userName) == 0 && p->isAdmin == 0)
                {
                    // 普通管理员：显示遮挡后的身份证号
                    maskIdCard(p->idCard, showIdCard);
                    // 表格式输出用户信息
                    printf("│%-50s│%-8s│%-19s│\n", p->userName, p->userId, showIdCard);
                    printf("├──────────────────────────────────────────────────┼────────┼───────────────────┤\n");
                    found++;
                }
                //超级管理员可以查找所有用户
                else if (g_currentUserSign == 2 && strcmp(name, p->userName) == 0)
                {
                    // 超级管理员：显示完整身份证号
                    strcpy(showIdCard, p->idCard);
                    // 表格式输出用户信息
                    printf("│%-50s│%-8s│%-19s│\n", p->userName, p->userId, showIdCard);
                    printf("├──────────────────────────────────────────────────┼────────┼───────────────────┤\n");
                    found++;
                }
                p = p->next;
            }
            printf("└──────────────────────────────────────────────────┴────────┴───────────────────┘\n");
            // 未找到用户
            if (!found)
            {
                showError("未找到相关用户！");
            }
        }
        //用户ID
        else if (choice == 2)
        {
            char id[UID_LEN] = "";
            printf("请输入用户ID：>");
            (void)scanf("%8s", id);
            // 清空输入缓冲区（处理残留字符）
            while (getchar() != '\n');

            // 标记是否找到用户
            int found = 0;
            printf("┌──────────────────────────────────────────────────┬────────┬───────────────────┐\n");
            printf("│%-50s│%-8s│%-19s│\n", "用户名", "用户ID", "身份证号");
            printf("├──────────────────────────────────────────────────┼────────┼───────────────────┤\n");
            while (p != NULL)
            {
                //普通管理员仅可查找读者
                if (g_currentUserSign == 1 && strcmp(id, p->userId) == 0 && p->isAdmin == 0)
                {
                    // 普通管理员：显示遮挡后的身份证号
                    maskIdCard(p->idCard, showIdCard);
                    // 表格式输出用户信息
                    printf("│%-50s│%-8s│%-19s│\n", p->userName, p->userId, showIdCard);
                    found++;
                }
                //超级管理员可以查找所有用户
                else if (g_currentUserSign == 2 && strcmp(id, p->userId) == 0)
                {
                    // 超级管理员：显示完整身份证号
                    strcpy(showIdCard, p->idCard);
                    // 表格式输出用户信息
                    printf("│%-50s│%-8s│%-19s│\n", p->userName, p->userId, showIdCard);
                    found++;
                }
                p = p->next;
            }
            printf("└──────────────────────────────────────────────────┴────────┴───────────────────┘\n");
            // 未找到用户
            if (!found)
            {
                showError("未找到相关用户！");
            }
        }
        else
        {
            clearScreen();
            continue;
        }
        //是否继续查找
        printf("\n是否继续查找用户？1-是 其余数字-否\n");
        int isContinue = getNumber();
        if (isContinue == 1)
        {
            clearScreen();
        }
        else
        {
            break;
        }
    }
}

/*************************************************
  Function:maskIdCard
  Description:身份证号隐私遮挡（保留前6位+后4位，中间8位用*代替）
  Calls:无
  Called By:searchUser
  Input:char* src - 原始18位身份证号
        char* dst - 输出遮挡后的身份证号（需提前分配≥19字节空间）
  Output:无
  Return:int - 1=成功，0=失败（输入为空/长度≠18）
  Others:仅处理18位身份证号，非18位直接返回失败
*************************************************/
int maskIdCard(char* src, char* dst)
{
    // 校验输入有效性
    if (src == NULL || dst == NULL || strlen(src) != 18)
    {
        showError("无效身份证号");
        return 0;
    }
    //复制前6位
    strncpy(dst, src, 6);
    //填充8个*
    memset(dst + 6, '*', 8);
    //复制后4位
    strncpy(dst + 14, src + 14, 4);
    //补字符串结束符
    dst[18] = '\0';

    return 1;
}


/*************************************************
  Function:borrowBook
  Description:借阅图书
  Calls:loadBooks,loadUsers,loadBorrows,centerPrint,
       clearScreen,getCurrentDateStr,saveBooks,
       saveBorrows,showError,showSuccess
  Called By:borrowManagementMenu
  Input:无
  Output:当前图书、借阅数据
  Return:无
  Others:无
*************************************************/
void borrowBook()
{
    User* userhead = NULL;
    Book* bookhead = NULL;
    Borrow* borrowhead = NULL;
    //是否成功借阅
    int hasSuccessedBorrow = 0;
    //获取书籍、读者、借阅信息链表头指针
    if (loadBooks() && loadUsers() && loadBorrows())
    {
        userhead = g_userHead;
        bookhead = g_bookHead;
        borrowhead = g_borrowHead;
    }
    else
    {
        return;
    }
    //循环借阅功能
    while(1)
    {
        //打印标题
        centerPrint(30, "借阅图书");
        Book* bookp = bookhead;
        User* userp = userhead;
        Borrow* borrowp = borrowhead;
        //直到获取到有效图书
        while(1)
        {
            bookp = bookhead;
            char isbn[ISBN_LEN] = "";
            printf("请输入ISBN编号(输入0则退出)\n");
            (void)scanf("%13s", isbn);
            // 清空输入缓冲区（处理残留字符）
            int ch = 0;
            while ((ch = getchar()) != '\n' && ch != EOF);
            while (bookp != NULL)
            {
                if (strcmp(isbn, bookp->ISBN) == 0)
                {
                    setColor(11);//亮青
                    printf("对应书籍为：\n");
                    printf("      书名：%s\n      作者：%s\n    出版社：%s\n      库存：%d\n"
                        , bookp->bookName, bookp->author, bookp->publisher, bookp->inventory);
                    setColor(15);
                    break;
                }
                bookp = bookp->next;
            }
            //找到图书
            if (bookp != NULL)
            {
                break;
            }
            //未找到图书
            else
            {
                showError("未找到对应图书，请重新输入！");
            }
        }
        //判断库存是否可以借阅
        //库存不能借阅
		if (bookp->inventory <= 0)
        {
            showError("当前图书库存数量为0，无法借阅！");
        }
        //库存可以借阅
        else
        {
            //直到读者信息输入正确
			while (1)
			{
				userp = userhead;
				char userid[UID_LEN] = "";
				printf("\n请输入用户ID：>");
				(void)scanf("%8s", userid);
                // 清空输入缓冲区（处理残留字符）
                int ch = 0;
                while ((ch = getchar()) != '\n' && ch != EOF);
				//遍历链表查找读者信息
				while (userp != NULL)
				{
					if (userp->isAdmin == 0 
                        && strcmp(userid, userp->userId) == 0)
					{
                        setColor(11);
                        printf("对应读者姓名为：%s\n", userp->userName);
                        setColor(15);
                        break;
					}
					userp = userp->next;
				}
                //找到对应读者信息
				if (userp != NULL)
				{
					break;
				}
				else
				{
                    showError("未找到对应用户，请重新输入！");
				}
			}
            //调用统计函数，获取该读者未归还借阅数
            int unreturnedCount = countUserBorrowedBooks(userp->userId);
            //处理统计函数返回错误
            if (unreturnedCount == -1)
            {
                showError("无法统计已借阅数量！");
                break;
            }
            //校验是否超过最大借阅数量
            if (unreturnedCount >= g_maxBorrowQuality)
            {
				setColor(12);//亮红
                printf("借阅失败：当前用户未归还图书%d本，已达到最大借阅数量%d本！",
                    unreturnedCount, g_maxBorrowQuality);
				setColor(15);
                break;
            }
			printf("\n是否确认借阅？1-是 其余数字-否\n");
			int confirm = getNumber();
            //确认借阅
			if (confirm == 1)
			{
                borrowp = borrowhead;
                //借阅记录遍历到最后一个
				while (borrowp != NULL && borrowp->next != NULL)
				{
					borrowp = borrowp->next;
				}
				//创建新的借阅记录节点
                Borrow* newBorrow = (Borrow*)creatNode(sizeof(Borrow));
                //如果开辟新内存成功
                if (newBorrow != NULL && userp != NULL && bookp != NULL)
                {
                    newBorrow->next = NULL;
					//拷贝图书、读者信息到借阅记录
                    strcpy(newBorrow->ISBN, bookp->ISBN);
                    strcpy(newBorrow->userId, userp->userId);
                    //此处写入时间
                    char nowTime[DATE_LEN] = "";
					//如果获取当前时间字符串成功则写入借阅记录并更新图书库存等信息
                    if (getCurrentDateStr(nowTime))
                    {
                        strcpy(newBorrow->borrowDate, nowTime);
                        newBorrow->isRenew = 0;
                        //写入应还日期
                        int currentargetYear, currentargetMonth, currentDay;
                        //解析当前日期字符串为数字（当前日期合法，无需判断返回值）
                        parseDateString(nowTime, &currentargetYear, &currentargetMonth, &currentDay);
                        //转换为总天数
                        long currentDays = dateToDays(currentargetYear, currentargetMonth, currentDay);
                        //计算到期总天数（当前天数 + 最大借阅天数）
                        long expireDays = currentDays + g_maxBorrowDays;
                        //总天数转回年月日
                        int expireYear, expireMonth, expireDay;
                        daysToDate(expireDays, &expireYear, &expireMonth, &expireDay);
                        //格式化到期日期，写入借阅记录
                        sprintf(newBorrow->returnDate, "%04d-%02d-%02d", expireYear, expireMonth, expireDay);

                        if (borrowhead != NULL)
                        {
                            borrowp->next = newBorrow;
                        }
                        else
                        {
                            borrowhead = newBorrow;
                        }
                        bookp->inventory--;//库存数-1
						bookp->borrowedCount++;//借阅数+1
						bookp->borrowTimes++;//总借阅数+1
						//标记成功借阅
                        hasSuccessedBorrow = 1;
                    }
					//获取当前时间字符串失败则释放新开辟的内存并提示错误
                    else
                    {
                        free(newBorrow);
                    }
                }
			}
            //不确认借阅
			else
			{
                showSuccess("借阅操作已取消！");
			}
        }
        printf("\n是否继续借阅图书？1-是 其余数字-否\n");
        int isContinue = getNumber();
        if (isContinue == 1)
        {
            clearScreen();
        }
        else
        {
            break;
        }
    }
    //如果成功借阅则保存图书和借阅信息
    if(hasSuccessedBorrow == 1)
    {
        (void)saveBooks(bookhead);
        (void)saveBorrows(borrowhead);
    }
}

/*************************************************
  Function:countUserBorrowedBooks
  Description:统计指定读者ID的未归还借阅图书数量
  Calls:loadBorrows,showError
  Called By:borrowBook
  Input:char* userId - 要统计的读者ID（非空字符串）
  Output:向标准输出打印：
        1. 错误提示（ID为空、加载借阅信息失败）；
        2. 统计结果（读者ID、未归还借阅本数）。
  Return:int - 成功返回未归还借阅数量（≥0），失败返回-1（ID为空/加载失败）
  Others:1. 依赖全局借阅链表头g_borrowHead；
         2. 仅匹配读者ID完全一致且isReturn=0（未归还）的借阅记录；
         3. 若读者无未归还记录，返回0并提示。
*************************************************/
int countUserBorrowedBooks(char* userId)
{
    //校验输入参数：读者ID不能为空
    if (userId == NULL || strlen(userId) == 0)
    {
        showError("统计失败：读者ID不能为空！");
        return -1;
    }

    //加载最新的借阅信息链表
    if (!loadBorrows())
    {
        showError("统计失败：加载借阅记录失败！");
        return -1;
    }

    //初始化变量：计数器、遍历指针
    int unreturnedCount = 0;// 未归还借阅数量
    Borrow* borrowp = g_borrowHead;//借阅链表遍历指针

    //遍历借阅链表
    while (borrowp != NULL)
    {
        //读者ID匹配 + 未归还（isReturn=0）
        if (strcmp(borrowp->userId, userId) == 0 && borrowp->isReturn == 0)
        {
            unreturnedCount++;//计数+1
        }
        borrowp = borrowp->next;//遍历下一个节点
    }
    //返回统计结果
    return unreturnedCount;
}

/*************************************************
  Function:isBookOverDue
  Description:判断归还书籍是否逾期（对比当前日期 vs 应还日期）
  Calls:getCurrentDateStr,parseDateString,dateToDays
  Called By:fineStatistics,returnBook
  Input:returnDate-借阅时预存的应还日期, intervalDay-输出：逾期天数（正数逾期，负数未逾期）
  Output:无
  Return:1=逾期，0=未逾期，-1=错误
  Others:无
*************************************************/
int isBookOverDue(const char* returnDate, int* intervalDay)
{
    int expireYear = 0, expireMonth = 0, expireDay = 0;
    int currentargetYear = 0, currentargetMonth = 0, currentDay = 0;
    char currentDate[DATE_LEN] = "";

    //获取当前时间字符串
    if (!getCurrentDateStr(currentDate))
    {
        return -1;
    }
    //解析【应还日期】字符串
    if (!parseDateString(returnDate, &expireYear, &expireMonth, &expireDay))
    {
        return -1;
    }
    //解析【当前日期】字符串
    if (!parseDateString(currentDate, &currentargetYear, &currentargetMonth, &currentDay))
    {
        return -1;
    }

    //转换为总天数
    long expireDays = dateToDays(expireYear, expireMonth, expireDay);
    long currentDays = dateToDays(currentargetYear, currentargetMonth, currentDay);

    //计算逾期天数：当前天数 - 应还天数（正数=逾期，负数=未逾期）
    long overDueDays = currentDays - expireDays;
    *intervalDay = (int)overDueDays;

    //返回逾期状态
    return overDueDays > 0 ? 1 : 0;
}

/*************************************************
  Function:returnBook
  Description:归还图书
  Calls:loadBooks,loadUsers,loadBorrows,centerPrint,showError
       ,getCurrentDateStr,isBookOverDue,setColor,showSuccess
  Called By:borrowManagementMenu
  Input:无
  Output:当前图书、借阅数据
  Return:无
  Others:无
*************************************************/
void returnBook()
{
    //是否成功归还
    int isReturn = 0;
    //读取书籍、借阅、用户数据
    Book* bookhead = NULL;
    User* userhead = NULL;
    Borrow* borrowhead = NULL;
    if (loadBooks() && loadUsers() && loadBorrows())
    {
        bookhead = g_bookHead;
        userhead = g_userHead;
        borrowhead = g_borrowHead;
    }
    else
    {
        return;
    }
    if (borrowhead == NULL)
    {
        centerPrint(30, "借阅图书");
        showError("当前没有借阅记录！");
        pauseScreen("按任意键返回…");
        return;
    }
    //循环归还
    while (1)
    {
        User* userp = NULL;
        Book* bookp = NULL;
        Borrow* borrowp = NULL;
        //打印标题
        centerPrint(30, "归还图书");
        //直到输入有效用户ID
        while(1)
        {
            char userid[UID_LEN] = "";
            printf("请输入用户ID：>");
            (void)scanf("%8s", userid);
            // 清空输入缓冲区（处理残留字符）
            while (getchar() != '\n');
            userp = userhead;
            while (userp != NULL)
            {
                if (strcmp(userp->userId, userid) == 0)
                {
                    setColor(11);//亮青
                    printf("对应用户名为：%s\n",userp->userName);
                    setColor(15);
                    break;
                }
                userp = userp->next;
            }
            if (userp != NULL)
            {
                break;
            }
            else
            {
                showError("未找到对应用户，请重新输入！");
            }
        }
        //直到查找到有效书籍
        while (1)
        {
            char isbn[ISBN_LEN] = "";
            printf("\n请输入ISBN编号：>");
            (void)scanf("%13s", isbn);
            // 清空输入缓冲区（处理残留字符）
            while (getchar() != '\n');
            bookp = bookhead;
            while (bookp != NULL)
            {
                if (strcmp(isbn, bookp->ISBN) == 0)
                {
                    setColor(11);//亮青
                    printf("对应书籍为：\n");
                    printf("      书名：%s\n      作者：%s\n    出版社：%s\n"
                        , bookp->bookName, bookp->author, bookp->publisher);
                    setColor(15);
                    break;
                }
                bookp = bookp->next;
            }
            //找到图书
            if (bookp != NULL)
            {
                break;
            }
            //未找到图书
            else
            {
                showError("未找到对应图书，请重新输入！");
            }
        }
        borrowp = borrowhead;
        while (borrowp != NULL)
        {
            //在借阅记录中找到ISBN、用户ID对应，并且未归还的图书
			if (strcmp(borrowp->ISBN, bookp->ISBN) == 0
				&& strcmp(borrowp->userId, userp->userId) == 0
				&& borrowp->isReturn == 0)
            {
                break;
            }
            borrowp = borrowp->next;
        }
        //同时存在有效借阅记录
        if (borrowp != NULL)
        {
            char nowTime[DATE_LEN] = "";
            //如果获取当前时间成功
            if (getCurrentDateStr(nowTime))
            {
                int interval = 0;
                if (isBookOverDue(borrowp->borrowDate, &interval) == 1)
                {
                    printf("\n本书应还日期为：%s\n当前已逾期：%d天\n"
                        , borrowp->returnDate, interval);
                    setColor(6);//黄色
                    printf("当前已经逾期！| 请缴纳罚金：%d元\n", interval*g_singleDayFine);
                    setColor(15);
                }
                printf("\n是否确认归还？1-是 其他数字-否\n");
                int confirm = getNumber();
                //确认归还图书
                if (confirm == 1)
                {
                    //标记归还
                    borrowp->isReturn = 1;
                    //记录归还时间
                    strcpy(borrowp->returnDate, nowTime);
                    //对应书籍库存增加
                    bookp->inventory++;
                    bookp->borrowedCount--;
                    //标记归还成功
                    isReturn = 1;
                }
                else
                {
                    showSuccess("归还操作已取消！");
                }
            }
            else
            {
                showError("归还失败！");
            }
        }
        else
        {
            showError("未查找到对应借阅记录");
        }
        printf("\n是否继续归还图书？1-是 其余数字-否\n");
        int choice = getNumber();
        if (choice == 1)
        {
            clearScreen();
        }
        else
        {
            break;
        }
    }
    //如果成功归还
	if (isReturn == 1)
    {
        (void)saveBorrows(borrowhead);
        (void)saveBooks(bookhead);
    }
}

/*************************************************
  Function:modifyReturnDate
  Description:批量修改归还日期：将所有早于指定日期的归还时间改为指定日期
  Calls:loadBorrows,parseDateString,dateToDays,
        showError,showSuccess,saveBorrows
  Called By:bookManagementMenu
  Input:无
  Output:修改结果提示
  Return:无
  Others:仅修改returnDate字段，不影响其他借阅数据
*************************************************/
void modifyReturnDate()
{
    //加载借阅记录链表
    if (!loadBorrows())
    {
        showError("加载借阅记录失败！");
        return;
    }
    Borrow* borrowHead = g_borrowHead;
    if (borrowHead == NULL)
    {
        showError("当前没有借阅记录！");
        return;
    }

    //获取并校验用户输入的指定日期
    char targetDate[DATE_LEN] = "";
    int targetYear, targetMonth, targetDay;
    centerPrint(32, "批量修改归还日期");
    while (1)
    {
        printf("请输入指定日期 (格式:YYYY-MM-DD)\n");
        (void)scanf("%10s", targetDate);
        //清空输入缓冲区
        int ch = 0;
        while ((ch = getchar()) != '\n' && ch != EOF);

        //校验日期合法性
        if (parseDateString(targetDate, &targetYear, &targetMonth, &targetDay))
        {
            break;
        }
        showError("日期格式或数值非法，请重新输入！");
    }

    //转换指定日期为总天数（用于对比）
    long targetDays = dateToDays(targetYear, targetMonth, targetDay);
    //统计修改条数
    int modifyCount = 0;
    Borrow* p = borrowHead;

    //二次确认
    setColor(96);
    printf("\n即将执行：将所有【归还日期早于%s】的记录，修改为%s\n", targetDate, targetDate);
    setColor(15);
    printf("确认执行？1-确认  其余数字-取消\n");
    int confirm = getNumber();
    if (confirm != 1)
    {
        showSuccess("批量修改操作已取消！");
        pauseScreen("请按任意键返回上级菜单…");
        return;
    }

    //遍历所有借阅记录，执行修改
    while (p != NULL)
    {
        if(p->isReturn == 0)
        {
            int borrowYear, borrowMonth, borrowDay;
            //解析当前记录的归还日期
            if (parseDateString(p->returnDate, &borrowYear, &borrowMonth, &borrowDay))
            {
                long recordDays = dateToDays(borrowYear, borrowMonth, borrowDay);
                //如果记录的归还日期 < 指定日期 则修改
                if (recordDays < targetDays)
                {
                    strcpy(p->returnDate, targetDate);
                    modifyCount++;
                }
            }
        }
        p = p->next;
    }

    //保存修改后的数据并提示结果
    if (modifyCount > 0)
    {
        saveBorrows(borrowHead);
        setColor(10);//设置控制台颜色为亮绿
        printf("成功修改 %d 条记录！\n", modifyCount);
        setColor(15);//还原控制台颜色
    }
    else
    {
        showError("未找到需要修改的借阅记录！");
    }
    pauseScreen("按任意键返回上级菜单…");
}

/*************************************************
  Function:modifyGlobalVars
  Description:修改部分全局变量
  Calls:centerPrint,clearScreen,getNumber,saveGlobalVars,showSuccess
  Called By:superAdminMenu
  Input:无显式函数参数；函数内部通过标准输入获取用户交互输入：
  Output:向输出打印内容：
  Return:无
  Others:1. 依赖全局变量：
            g_tryMaxTimes(登录最大尝试次数)、g_maxBorrowDays(最长借阅天数)、
            g_singleDayFine(每本单日罚金)、g_maxBorrowQuality(最多借阅数量)；
         2. 修改过程中会将原始全局变量值暂存到临时变量，用户取消修改时自动恢复原值；
         3. 仅当至少有一次确认修改（isModify=1）时，才执行保存操作；
         4. 所有用户输入通过getNumber函数获取，避免非数字输入导致的程序异常。
*************************************************/
void modifyGlobalVars()
{
    //是否成功修改
    int isModify = 0;
    //循环修改
    while (1)
    {
        //暂存当前信息
        int tempTryTime = g_tryMaxTimes;
        int tempBorrowDay = g_maxBorrowDays;
        int tempSingleFine = g_singleDayFine;
        int tempBorrowQuality = g_maxBorrowQuality;
        int tempRenewDays = g_renewDays;
        //打印标题
        centerPrint(26,"修改设定");
        //打印当前数据
        printf("┌───────────────┬────────┐\n");
        printf("│%-15s│%-8s│\n", "选项", "信息");
        printf("├───────────────┼────────┤\n");
        printf("│%-15s│%-6d%s│\n", "1.登录尝试次数", g_tryMaxTimes, "次");
        printf("├───────────────┼────────┤\n");
        printf("│%-15s│%-6d%s│\n", "2.最长借阅天数", g_maxBorrowDays, "天");
        printf("├───────────────┼────────┤\n");
        printf("│%-15s│%-6d%s│\n", "3.续借天数", g_renewDays, "天");
        printf("├───────────────┼────────┤\n");
        printf("│%-15s│%-6d%s│\n", "4.每本单日罚金", g_singleDayFine, "元");
        printf("├───────────────┼────────┤\n");
        printf("│%-15s│%-6d%s│\n", "5.最多借阅数量", g_maxBorrowQuality, "本");
        printf("├───────────────┼────────┤\n");
        printf("│%-15s│%-8s│\n", "其他数字", "退出");
        printf("└───────────────┴────────┘\n");
        printf("请输入选项：>");
        int choice = getNumber();
		int newValue = 0;
        if (choice == 1)
        {
			while (1)
            {
                printf("请输入新次数：>");
                (void)scanf("%d", &newValue);
                if (newValue < 1)
                {
                    showError("登录尝试次数必须大于0！");
                }
                else
                {
                    g_tryMaxTimes = newValue;
					break;
                }
            }
        }
        else if (choice == 2)
        {
			while (1)
            {
                printf("请输入新天数：>");
                (void)scanf("%d", &newValue);
                if (newValue < 1)
                {
                    showError("最长借阅天数必须大于0！");
                }
                else
                {
                    g_maxBorrowDays = newValue;
					break;
                }
            }
        }
        else if (choice == 3)
        { 
            while (1)
            {
                printf("请输入新天数：>");
                (void)scanf("%d", &newValue);
                if (newValue < 1)
                {
                    showError("续借天数必须大于0！");
                }
                else
                {
                    g_renewDays = newValue;
					break;
                }
            }
        }
        else if (choice == 4)
        {
			while (1)
            {
                printf("请输入新金额：>");
                (void)scanf("%d", &newValue);
                if (newValue < 0)
                {
                    showError("每本单日罚金必须大于或等于0！");
                }
                else
                {
                    g_singleDayFine = newValue;
					break;
                }
            }
        }
        else if (choice == 5)
        {
			while (1)
            {
                printf("请输入新数量：>");
                (void)scanf("%d", &newValue);
                if (newValue < 1)
                {
                    showError("最多借阅数量必须大于0！");
                }
                else
                {
                    g_maxBorrowQuality = newValue;
                    break;
                }
            }
        }
        else
        {
            break;
        }
        printf("\n是否确认修改？1-是 其余数字-否\n");
        int confirm = getNumber();
        if (confirm == 1)
        {
            isModify = 1;
            showSuccess("修改成功！");
        }
        else
        {
            g_tryMaxTimes = tempTryTime;
            g_maxBorrowDays = tempBorrowDay;
            g_singleDayFine = tempSingleFine;
			g_maxBorrowQuality = tempBorrowQuality;
            g_renewDays = tempRenewDays;
            showSuccess("修改操作已取消！已恢复原有配置");
        }
        printf("\n是否继续修改设定？1-是 其他数字-否\n");
        int isContinue = getNumber();
        if (isContinue == 1)
        {
            clearScreen();
        }
        else
        {
            break;
        }
    }
    //如果成功修改则保存
    if (isModify == 1)
    {
        (void)saveGlobalVars();
    }
}