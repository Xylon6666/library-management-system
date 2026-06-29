#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "library.h"

/*************************************************
  Function:fileExists
  Description:检查文件是否存在
  Calls:setColor
  Called By:isFirstRun,normalRun 
  Input:filePath-要检查的文件路径
  Output:无
  Return:1-存在 0-不存在 -1-打开失败
  Others:无
*************************************************/
int fileExists(const char* filePath)
{
	//尝试以只读方式打开文件
    FILE* fp = fopen(filePath, "r");
    //文件成功打开
    if (fp != NULL)
    {
        fclose(fp);
        return 1;
    }
    //文件不存在
    //Error NO ENTity（“实体-文件或目录-不存在”）
    else if (errno == ENOENT) 
    {
        return 0;
    }
    //文件打开失败
    else
    {
        setColor(12);//亮红
        printf("检查文件%s失败：%s\n", filePath, strerror(errno));
        setColor(15);
        pauseScreen("按下任意键继续…");
        return -1;
    }
}

/*************************************************
  Function:isFirstRun
  Description:判断是否第一次运行程序
  Calls:fileExists
  Called By:main
  Input:无
  Output:无
  Return:1-第一次运行 0-非第一次运行
  Others:判断标准为data.dat是否存在
*************************************************/
int isFirstRun()
{
    int dataExist = fileExists(FILE_DATA);
    //读取出错返回-1
    if (dataExist == -1)
    {
        return -1;
    }
    //是第一次运行返回1，不是返回0
    return (dataExist == 0) ? 1 : 0;
}

/*************************************************
  Function:firstRun
  Description:首次运行需要执行的程序
  Calls:setColor,showSuccess，pauseScreen
  Called By:main
  Input:无
  Output:初始化过后的user.dat
  Return:0-失败 1-成功
  Others:无
*************************************************/
int firstRun()
{
    setColor(47);//绿底白字
    printf("【程序首次运行】正在初始化……\n\n");
    setColor(15);
    //创建data.dat并写入数据
    FILE* fpData = fopen(FILE_DATA, "wb");
    if (fpData == NULL) 
    {
        setColor(12);//亮红
        printf("创建data.dat失败！原因为：%s\n", strerror(errno));
        setColor(15);
        pauseScreen("按下任意键退出程序…");
        return 0;
    }
    fwrite(&g_tryMaxTimes, sizeof(int), 1, fpData);
    fwrite(&g_maxBorrowDays, sizeof(int), 1, fpData);
    fwrite(&g_singleDayFine, sizeof(int), 1, fpData);
	fwrite(&g_maxBorrowQuality, sizeof(int), 1, fpData);
    fclose(fpData);
    showSuccess("成功创建data.dat并写入默认数据");

    //创建并初始化user.dat
    FILE* fpUser = fopen(FILE_USER, "wb");
    if (fpUser == NULL) 
    {
        setColor(12);//亮红
        printf("创建user.dat失败！原因：%s\n", strerror(errno));
        setColor(15);
        //删除已创建的data.dat，避免下次误判为非首次
        remove(FILE_DATA);
        pauseScreen("按下任意键退出程序…");
        return 0;
    }
    User temp = { "00000001","888888","超级管理员","000000000000000000",2,NULL };
    fwrite(&temp,sizeof(temp), 1, fpUser);
    fclose(fpUser);

    //创建图书数据文件books.txt
    FILE* fpBook = fopen(FILE_BOOK, "wb");
    if (fpBook == NULL)
    {
        setColor(12);//亮红
        printf("创建books.txt失败！原因：%s\n", strerror(errno));
        setColor(15);
        //回滚：删除已创建的所有文件，保证首次运行判断
        remove(FILE_DATA);
        remove(FILE_USER);
        pauseScreen("按下任意键退出程序…");
        return 0;
    }
    fclose(fpBook);
    showSuccess("成功创建books.txt（图书数据文件）");

    //创建借阅数据文件borrow.txt
    FILE* fpBorrow = fopen(FILE_BORROW, "wb");
    if (fpBorrow == NULL)
    {
        setColor(12);//亮红
        printf("创建borrow.txt失败！原因：%s\n", strerror(errno));
        setColor(15);
        //回滚：删除已创建的所有文件，保证首次运行判断
        remove(FILE_DATA);
        remove(FILE_USER);
        remove(FILE_BOOK);
        pauseScreen("按下任意键退出程序…");
        return 0;
    }
    fclose(fpBorrow);
    showSuccess("成功创建borrow.txt（借阅数据文件）");

    showSuccess("成功创建user.txt并写入超级管理员信息");
    setColor(96);//黄底黑字
    printf("\n>>超级管理员信息<<\n用户ID  ：00000001\n默认密码：888888  \n");
    setColor(15);
    pauseScreen("按下任意键进入登录界面…");
    return 1;
}

/*************************************************
  Function:normalRun
  Description:非首次运行时的检查程序
  Calls:setColor,showSuccess,showError,pauseScreen
  Called By:      // 调用本函数的函数清单
  Input:无
  Output:无
  Return:1-正常 0-出现错误
  Others:无
*************************************************/
int normalRun()
{
    setColor(47);//绿底白字
    printf("【非首次运行】加载数据并检测文件中……\n");
    setColor(15);
    //读取data.dat的全局变量
    FILE* fpData = fopen(FILE_DATA, "rb");
    if (fpData == NULL) 
    {
        setColor(12);//亮红
        printf("读取data.dat失败！原因：%s\n", strerror(errno));
        setColor(15);
        pauseScreen("按下任意键继续…");
        return 0;
    }
    fread(&g_tryMaxTimes, sizeof(int), 1, fpData);
    fread(&g_maxBorrowDays, sizeof(int), 1, fpData);
    fread(&g_singleDayFine, sizeof(int), 1, fpData);
    fclose(fpData);
    showSuccess("数据读取成功！");
    //检查user.txt不存在，不存在则报错退出
    int userExist = fileExists(FILE_USER);
    if (userExist == 0) 
    {
        showError("错误：核心文件user.txt不存在！");
        pauseScreen("按下任意键退出程序…");
        return 0;
    }
    else if (userExist == 1) 
    {
        showSuccess("核心数据文件存在，验证通过!");
    }
    //检测book.txt、borrow.txt是否存在
    if (fileExists(FILE_BOOK) == 0) 
    {
        setColor(4);//红色
        printf("警告：book.txt不存在，请联系管理员确认！\n");
        setColor(15);
    }
    if (fileExists(FILE_BORROW) == 0) 
    {
        setColor(4);//红色
        printf("警告：borrow.txt不存在，请联系管理员确认！\n");
        setColor(15);
    }
    pauseScreen("按下任意键进入登录界面…");
    return 1;
}

/*************************************************
  Function:saveGlobalVars
  Description:将系统可配置的全局参数以二进制形式保存到data.dat文件
  Calls:fopen,fwrite,fclose,setColor,printf,strerror,pauseScreen
  Called By:modifyGlobalVars
  Input:无输入参数
  Output:保存失败时，控制台打印亮红色的错误提示信息（含具体失败原因）
  Return:1-全局变量保存成功，0-文件打开失败/保存失败
  Others:保存的全局参数包括：登录最大尝试次数(g_tryMaxTimes)、
         书籍最大借阅天数(g_maxBorrowDays)、单本书单日违约金(g_singleDayFine)、
         用户最大借阅数量(g_maxBorrowQuality)；
*************************************************/
int saveGlobalVars() 
{
    FILE* fp = fopen(FILE_DATA, "wb");
    if (fp == NULL) 
    {
        setColor(12);//亮红
        printf("保存全局变量失败！原因：%s\n", strerror(errno));
        setColor(15);
        pauseScreen("按下任意键继续…");
        return 0;
    }
    fwrite(&g_tryMaxTimes, sizeof(int), 1, fp);
    fwrite(&g_maxBorrowDays, sizeof(int), 1, fp);
    fwrite(&g_singleDayFine, sizeof(int), 1, fp);
	fwrite(&g_maxBorrowQuality, sizeof(int), 1, fp);
    fclose(fp);
    return 1;
}

/*************************************************
  Function:通用节点创建函数
  Description:适配所有结构体的节点创建
  Calls:无
  Called By:loadBooks,loadBorrows,loadBooks
  Input:nodeSize - 要创建的结构体节点的大小（sizeof(结构体)）
  Output:分配并初始化好的通用节点指针
  Return:NULL-创建失败 非NULL-创建成功（需强制类型转换）
  Others:本函数是万能创建函数，所有链表节点都可复用
*************************************************/
void* creatNode(size_t nodeSize)
{
    //尝试开辟内存
    void* node = malloc(nodeSize);
    //开辟失败的话提示并返回
    if (node == NULL)
    {
        showError("链表节点创建失败！");
        pauseScreen("按任意键继续…");
        return node;
    }
    //开辟成功后置空所有数值
    memset(node, 0, nodeSize);
    //返回开辟内存的指针
    return node;
}

/*************************************************
  Function:loadBooks
  Description:从文件读取图书数据到内存
  Calls:creatBookNode(),freeBookList();
  Called By:      //调用本函数的函数清单
  Input:无
  Output:图书链表的头节点，是否成功标志
  Return: 1-成功 0-失败
  Others:文件中每行格式为【ISBN 图书名 作者 库存】，图书名和作者名无空格；
*************************************************/
int loadBooks()
{
    //尝试以只读的方式打开文件
    FILE* bookfp = fopen(FILE_BOOK,"r");
    //如果打开失败提示并返回0
    if (bookfp == NULL)
    {
        printf("图书文件打开失败！错误原因：%s\n", strerror(errno));
        pauseScreen("按任意键继续…");
        return 0;
    }
    char ISBN[ISBN_LEN];     //图书编号
    char bookName[NAME_LEN]; //图书名
    char author[AUTHOR_LEN];   //作者
    char publisher[NAME_LEN];// 出版社
    int inventory;           // 库存数量
    int borrowedCount;       // 已借出数量
    int borrowTimes;         // 累计借阅次数

    Book* head = NULL;       //图书链表头节点指针
    Book* tail = NULL;       //图书链表尾节点指针

    //读取图书文件中有效数据并建立链表
	while (fscanf(bookfp, "%s %s %s %s %d %d %d"
		, ISBN, bookName, author, publisher, &inventory, &borrowedCount, &borrowTimes) == 7)
    {
        //尝试开辟内存
        Book* booknode = (Book*)creatNode(sizeof(Book));
        //内存开辟成功
        if (booknode != NULL)
        {
            memset(booknode, 0, sizeof(Book));
            //给手动创建的字符串变量添加\0以消除编译器警告
            ISBN[ISBN_LEN - 1] = '\0';
            bookName[NAME_LEN - 1] = '\0';
            author[AUTHOR_LEN - 1] = '\0';
            publisher[NAME_LEN - 1] = '\0';
            //图书链表节点数据处理
            strncpy(booknode->ISBN, ISBN, ISBN_LEN - 1);
            strncpy(booknode->bookName, bookName, NAME_LEN - 1);
            strncpy(booknode->author, author, AUTHOR_LEN - 1);
            strncpy(booknode->publisher, publisher, NAME_LEN - 1);
            booknode->inventory = inventory;
            booknode->borrowedCount = borrowedCount;
            booknode->borrowTimes = borrowTimes;
            booknode->next = NULL;

            //链表链接
            if (head == NULL)
            {
                head = booknode;
                tail = booknode;
            }
            else
            {
                tail->next = booknode;
                tail = booknode;
            }
        }
        //内存开辟失败
        else
        {
            showError("图书链表创建失败！");
            freeBookList(head);
            fclose(bookfp);
            pauseScreen("按任意键继续…");
            return 0;
        }
    }
    //如果读取过程中出现错误
    if (ferror(bookfp))
    {
        setColor(12);
        printf("图书文件读取过程中发生IO错误！错误原因：%s\n", strerror(errno));
        setColor(15);
        freeBookList(head); //读取错误，释放已创建的链表
        head = NULL;        //置空，保证返回NULL
        fclose(bookfp);
        pauseScreen("按任意键继续…");
        return 0;
    }
    fclose(bookfp);
    g_bookHead = head;
    return 1;
}

/*************************************************
  Function:保存图书
  Description:把内存图书数据写入文件
  Calls:freeBookList()
  Called By:      //调用本函数的函数清单
  Input:head-图书链表的头节点指针，NULL表示空链表（清空文件）
  Output:将当前最新的图书数据保存到txt文件
  Return:0-写入过程中出现错误 1-写入成功
  Others:写入格式与loadBooks读取格式一致【ISBN 图书名 作者 库存】，一行一个；
         采用"w"模式打开文件，会清空原有所有内容后重新写入
*************************************************/
int saveBooks(Book* head)
{
    Book* p = head;
    //打开指定的文件
    FILE* bookfp = fopen(FILE_BOOK, "w");
    if (bookfp == NULL)
    {
        setColor(12);
        printf("图书文件打开失败！错误原因：%s\n", strerror(errno));
        setColor(12);
        pauseScreen("按任意键继续…");
        return 0;
    }
    //设置文件为ANSI（O_TEXT）模式，强制按系统GBK编码处理字符
    if (_setmode(_fileno(bookfp), _O_TEXT) == -1) 
    {
        printf("设置编码失败：%s\n", strerror(errno));
        fclose(bookfp);
        pauseScreen("按任意键继续…");
        return 0;
    }
    int writeOk = 1;//写入过程中是否出现错误
    //遍历链表写入数据
    while (p != NULL)
    {
        //判断fprintf写入是否成功，<0表示写入失败
        if (fprintf(bookfp, "%s %s %s %s %d %d %d\n"
            , p->ISBN, p->bookName, p->author, p->publisher, p->inventory, p->borrowedCount, p->borrowTimes) < 0)
        {
            setColor(12);
            printf("图书数据写入失败！错误原因：%s\n", strerror(errno));
            setColor(15);
            writeOk = 0;
            pauseScreen("按任意键继续…");
            break;
        }
        p = p->next;
    }
    fclose(bookfp);//关闭文件
    freeBookList(head);//释放链表
    //如果写入过程中没有错误就提示成功提示
    if (writeOk)
    {
        showSuccess("图书数据保存成功！");
    }
    //返回，退出函数
    return 1;
}

/*************************************************
  Function:释放图书链表
  Description:释放图书链表
  Calls:无
  Called By:loadBooks,saveBooks
  Input:需要释放的链表头节点指针
  Output:无
  Return:无
  Others:无
*************************************************/
void freeBookList(Book* head)
{
    Book* p = head;
    while (p != NULL)
    {
        head = head->next;//先保存下一个节点
        free(p);//释放当前节点
        p = head;//p指向下一个要释放的节点
    }
}

/*************************************************
  Function:读取用户数据
  Description:从二进制用户文件读取数据到内存，还原为用户链表
  Calls:creatUserNode()、freeUserList()
  Called By:      //调用本函数的函数清单
  Input:无
  Output:用户链表的头节点指针
  Return:1-成功 0-失败
  Others:采用二进制rb模式，按整块User结构体读取，自动还原所有字段；
         读取失败会检测IO错误
*************************************************/
int loadUsers()
{
    //尝试以二进制的方式打开用户文件
    FILE* userfp = fopen(FILE_USER, "rb");
    //如果打开失败
    if (userfp == NULL)
    {
        setColor(12);
        printf("用户文件打开失败！错误原因：%s\n", strerror(errno));
        setColor(15);
        pauseScreen("按任意键继续…");
        return 0;
    }
    //打开成功
    //创建需要的变量
    User* head = NULL;       //用户链表头节点
    User* tail = NULL;       //用户链表尾节点
    User tempUser;           //临时用户结构体：接收fread读取的数据
    memset(&tempUser, 0, sizeof(User)); //初始化临时结构体
    //读取到文件尾
    while (fread(&tempUser, sizeof(User), 1, userfp) == 1)
    {
        //尝试开辟新内存
        User* usernode = (User*)creatNode(sizeof(User));
        //如果内存开辟成功
        if (usernode != NULL)
        {
            //将临时结构体的内容复制到新节点
            memcpy(usernode, &tempUser, sizeof(User));
            usernode->next = NULL;
            //如果头指针为空
            if (head == NULL)
            {
                head = usernode;
                tail = usernode;
            }
            //如果头指针不为空
            else
            {
                tail->next = usernode;
                tail = usernode;
            }
        }
        //如果内存开辟失败
        else
        {
            showError("用户链表节点创建失败！");
            freeUserList(head);
            fclose(userfp);
            pauseScreen("按任意键继续…");
            return 0;
        }
        memset(&tempUser, 0, sizeof(User)); //清空临时结构体，准备下一次读取
    }
    // 检测读取过程中的IO错误
    if (ferror(userfp))
    {
        setColor(12);
        printf("用户文件二进制读取过程中发生IO错误！错误原因：%s\n", strerror(errno));
        setColor(15);
        freeUserList(head);
        head = NULL;
        fclose(userfp);
        pauseScreen("按任意键继续…");
        return 0;
    }
    //关闭文件
    fclose(userfp);
    //运行到此处已经成功读取
    //将全局用户链表头指针赋值为当前创建的头指针
    g_userHead = head;
    return 1;
}

/*************************************************
  Function:保存用户数据
  Description:把内存中用户链表以二进制形式写入文件，密码不可见
  Calls:freeUserList()
  Called By:
  Input:head-用户链表头节点指针，NULL表示空链表
  Output:无
  Return:0-写入过程中出现错误 1-写入成功
  Others:采用二进制wb模式，按整块User结构体写入，文件为乱码不可读；
         写入失败会检测IO错误
*************************************************/
int saveUsers(User* head)
{
    // 空链表处理：打开文件后直接关闭（清空文件），视为保存成功
    if (head == NULL)
    {
        FILE* userfp = fopen(FILE_USER, "wb");
        if (userfp == NULL)
        {
            setColor(12);
            printf("用户文件打开失败！错误原因：%s\n", strerror(errno));
            setColor(15);
            pauseScreen("按任意键继续…");
            return 0;
        }
        fclose(userfp);
        showError("用户链表为空！");
        pauseScreen("按任意键继续…");
        return 1;
    }
    //尝试以二进制写的方式打开用户文件
    User* p = head;
    FILE* userfp = fopen(FILE_USER, "wb");
    //若打开失败
    if (userfp == NULL)
    {
        setColor(12);
        printf("用户文件打开失败！错误原因：%s\n", strerror(errno));
        setColor(15);
        pauseScreen("按任意键继续…");
        return 0;
    }
    int writeOk = 1;//写入过程是否出现错误
    //遍历链表
    while (p != NULL)
    {
        //如果写入失败
        if (fwrite(p, sizeof(User), 1, userfp) != 1)
        {
            setColor(12);
            printf("用户数据二进制写入失败！错误原因：%s\n", strerror(errno));
            setColor(15);
            writeOk = 0;
            pauseScreen("按任意键继续…");
            break;
        }
        p = p->next;
    }
    fclose(userfp);//关闭文件
    freeUserList(head);//释放链表
    //如果写入过程中没有出现问题则输出成功提示
    if (writeOk)
    {
        showSuccess("用户数据保存成功！");
    }
    return 1;
}

/*************************************************
  Function:释放用户链表
  Description:释放用户链表
  Calls:无
  Called By:loadUsers(),saveUser()
  Input:需要释放的链表头节点指针
  Output:无
  Return:无
  Others:无
*************************************************/
void freeUserList(User* head)
{
    User* p = head;
    while (p != NULL)
    {
        head = head->next;// 先保存下一个节点
        free(p);// 释放当前节点
        p = head;//p指向下一个要释放的节点
    }
}

/*************************************************
  Function:释放借阅链表
  Description:释放借阅链表所有节点的动态内存，防止内存泄漏
  Calls:无
  Called By:loadBorrows、saveBorrows
  Input:head-借阅链表头节点指针
  Output:无
  Return:无
  Others:释放前先保存下一个节点，避免访问已释放内存的野指针
*************************************************/
void freeBorrowList(Borrow* head)
{
    Borrow* p = head;
    while (p != NULL)
    {
        head = head->next;//先保存下一个节点
        free(p);//释放当前节点
        p = head;//p移动到下一个节点
    }
}

/*************************************************
  Function:读取借阅信息
  Description:从borrow.txt文本文件读取借阅数据到内存，构建借阅链表
  Calls:creatBorrowNode()、freeBorrowList()
  Called By:
  Input:无
  Output:借阅链表的头节点指针,是否成功标志
  Return:1-成功 0-失败
  Others:文件中每行格式【userId ISBN 借阅日期 归还日期 归还状态】；
         单空格分隔，日期格式YYYY-MM-DD，未归还则归还日期为0000-00-00；
         归还状态0=未归还，1=已归还
*************************************************/
int loadBorrows()
{
    //尝试以只读的方式打开文件
    FILE* bfp = fopen(FILE_BORROW, "r");
    //如果打开失败
    if (bfp == NULL)
    {
        setColor(12);
        printf("借阅文件打开失败！错误原因：%s\n", strerror(errno));
        setColor(15);
        pauseScreen("按任意键继续…");
        return 0;
    }
    //创建读取文件时需要的临时变量
    char userId[UID_LEN];
    char ISBN[ISBN_LEN];
    char borrowDate[DATE_LEN];
    char returnDate[DATE_LEN];
    int isRenew;
    int isReturn;
    //尾插法创建用户列表时需要的首尾指针
    Borrow* head = NULL;
    Borrow* tail = NULL;
    //读取到文件尾
    while (fscanf(bfp, "%s %s %s %s %d %d",
        userId, ISBN, borrowDate, returnDate,&isRenew, &isReturn) == 6)
    {
        //尝试开辟内存
        Borrow* bnode = (Borrow*)creatNode(sizeof(Borrow));
        //如果内存开辟成功
        if (bnode != NULL)
        {
            //将开辟的内存数据设置为0
            memset(bnode, 0, sizeof(Borrow));
            //手动为创建的临时字符字符串变量添加\0以消除编译器警告
            userId[UID_LEN - 1] = '\0';
            ISBN[ISBN_LEN - 1] = '\0';
            borrowDate[DATE_LEN - 1] = '\0';
            returnDate[DATE_LEN - 1] = '\0';
            //将读取到的信息写入开辟好的结构体
            strncpy(bnode->userId, userId, UID_LEN - 1);
            strncpy(bnode->ISBN, ISBN, ISBN_LEN - 1);
            strncpy(bnode->borrowDate, borrowDate, DATE_LEN - 1);
            strncpy(bnode->returnDate, returnDate, DATE_LEN - 1);
            bnode->isRenew = isRenew;
            bnode->isReturn = isReturn; 
            bnode->next = NULL;
            //如果链表头指针为空
            if (head == NULL)
            {
                head = bnode;
                tail = bnode;
            }
            //链表头指针不为空
            else
            {
                tail->next = bnode;
                tail = bnode;
            }
        }
        //内存开辟失败
        else
        {
            showError("借阅链表节点创建失败！");
            freeBorrowList(head);
            fclose(bfp);
            pauseScreen("按任意键继续…");
            return 0;
        }
    }
    //如果文件读取过程出现错误
    if (ferror(bfp))
    {
        setColor(12);
        printf("借阅文件读取过程中发生IO错误！错误原因：%s\n", strerror(errno));
        setColor(15);
        freeBorrowList(head);
        head = NULL;
        fclose(bfp);
        pauseScreen("按任意键继续…");
        return 0;
    }
    //关闭文件
    fclose(bfp);
    //运行到此处则读取和创建没有出现任何问题
    //将去全局借阅数据头指针设置为当前链表头指针
    g_borrowHead = head;
    return 1;
}

/*************************************************
  Function:保存借阅信息
  Description:把内存中借阅链表以文本形式写入文件，覆盖原有内容
  Calls:freeBorrowList()
  Called By:
  Input:head-借阅链表头节点指针，NULL表示空链表（清空文件）
  Output:无
  Return:0-写入过程中出现错误 1-写入成功
  Others:写入格式【userId ISBN 借阅日期 归还日期 归还状态】，单空格分隔；
         采用"w"模式打开文件；写入失败检测IO错误
*************************************************/
int saveBorrows(Borrow* head)
{
    // 空链表处理：打开文件后直接关闭，清空原有内容，视为保存成功
    if (head == NULL)
    {
        FILE* bfp = fopen(FILE_BORROW, "w");
        //如果文件打开失败
        if (bfp == NULL)
        {
            setColor(12);
            printf("借阅文件打开失败！错误原因：%s\n", strerror(errno));
            setColor(15);
            pauseScreen("按任意键继续…");
            return 0;
        }
        fclose(bfp);
        showError("借阅链表为空");
        pauseScreen("按任意键继续…");
        return 1;
    }
    Borrow* p = head;
    //尝试以只写的方式打开文件
    FILE* bfp = fopen(FILE_BORROW, "w");
    //如果文件打开失败
    if (bfp == NULL)
    {
        setColor(12);
        printf("借阅文件打开失败！错误原因：%s\n", strerror(errno));
        setColor(15);
        pauseScreen("按任意键继续…");
        return 0;
    }
    //关键：设置文件为ANSI（O_TEXT）模式，强制按系统GBK编码处理字符
    if (_setmode(_fileno(bfp), _O_TEXT) == -1)
    {
        printf("设置编码失败：%s\n", strerror(errno));
        fclose(bfp);
        pauseScreen("按任意键继续…");
        return 0;
    }
    // 写入失败标记：检测IO错误
    int writeOk = 1;
    //遍历链表
    while (p != NULL)
    {
        if (fprintf(bfp, "%s %s %s %s %d %d\n",
            p->userId, p->ISBN, p->borrowDate, p->returnDate,p->isRenew, p->isReturn) < 0)
        {
            setColor(12);
            printf("借阅数据写入失败！错误原因：%s\n", strerror(errno));
            setColor(15);
            writeOk = 0;
            pauseScreen("按任意键继续…");
            break;
        }
        p = p->next;
    }
    //关闭文件
    fclose(bfp);
    //释放链表
    freeBorrowList(head);
    //如果写入过程中没有出现错误则输出成功提示
    if (writeOk)
    {
        showSuccess("借阅信息保存成功！");
    }
    return 1;
}

/*************************************************
  Function: saveListToCsv
  Description: 将图书/借阅数据链表导出为CSV文件
  Calls: loadBooks(), loadBorrow(), freeBookList(), freeBorrowList()
  Called By: 
  Input: choice -> 1-图书数据 2-借阅数据
  Output: 指定路径的CSV文件
  Return: 无
  Others: 1. 路径输入限制长度，避免缓冲区溢出
          2. 严格保证资源释放顺序，避免内存泄漏
          3. 增加成功/失败提示，提升用户体验
*************************************************/
void saveListToCsv(int choice)
{
    //接收用户想要输出的文件路径
    char filedest[MAX_PATH] = { 0 };
    printf("请输入CSV文件导出路径（如C:/XXX/你想要的文件名.csv）：\n");
    setColor(11);
    printf("注意：请使用 / 这个符号，最后一定要加上.csv，勿输入空格！\n");
    setColor(15);
    (void)scanf("%259s", filedest);
    //尝试以只写的方式打开文件
    FILE* fp = fopen(filedest, "w");
    //如果文件打开失败
    if (fp == NULL)
    {
        setColor(12);
        printf("新文件打开失败！错误原因：%s\n", strerror(errno));
        setColor(15);
        pauseScreen("按任意键继续…");
        return;
    }
    int exportSuccess = 1;//检测文件写入过程是否出现错误
    if (choice == 1)//导出图书数据
    {
        Book* head = NULL;
        //读取图书数据
        //读取成功
        if (loadBooks())
        {
            head = g_bookHead;
        }
        //读取失败
        else
        {
            return;
        }
        Book* p = head;
        //如果读取出来的图书指针为空-当前没有图书数据
        if (p == NULL)
        {
            showError("当前没有图书数据！");
            freeBookList(head);
            fclose(fp);
            pauseScreen("按任意键继续…");
            return;
        }
        //先写入表头
        fprintf(fp, "ISBN编码,书名,作者,出版社,库存数量,已借出数量,累计借阅次数\n");
        //遍历链表
        while (p != NULL)
        {
            //如果写入过程出错
            if (fprintf(fp, "%s,%s,%s,%s,%d,%d,%d\n",
                p->ISBN, p->bookName, p->author, p->publisher, p->inventory, p->borrowedCount, p->borrowTimes) < 0)
            {
                setColor(12);
                printf("图书数据写入文件失败！错误原因：%s\n", strerror(errno));
                setColor(15);
                exportSuccess = 0;
                pauseScreen("按任意键继续…");
                break;
            }
            p = p->next;
        }
        //释放链表
        freeBookList(head);
    }
    else if (choice == 2)//导出借阅数据
    {
        Borrow* head = NULL;
        //读取借阅数据
        //读取成功
        if (loadBorrows())
        {
            head = g_borrowHead;
        }
        //读取失败
        else
        {
            return;
        }
        Borrow* p = head;
        //如果读取出来的链表头指针为空-当前没有借阅数据
        if (p == NULL)
        {
            showError("当前没有借阅数据！");
            fclose(fp);
            freeBorrowList(head);
            pauseScreen("按任意键继续…");
            return;
        }
        //写入表头
        fprintf(fp, "用户ID,ISBN编码,借阅时间,归还时间,是否归还\n");
        //遍历链表
        while (p != NULL)
        {
            char status[7] = "";
            if (p->isReturn == 1)
            {
                strcpy(status, "已归还");
            }
            else
            {
                strcpy(status, "未归还");
            }
            //如果写入出错
            if (fprintf(fp, "%s,%s,%s,%s,%s\n",
                p->userId, p->ISBN, p->borrowDate, p->returnDate, status) < 0)
            {
                setColor(12);
                printf("借阅数据写入文件失败！错误原因：%s\n", strerror(errno));
                setColor(15);
                pauseScreen("按任意键继续…");
                break;
            }
            p = p->next;
        }
        //释放链表
        freeBorrowList(head);
    }
    else if (choice == 3)//导出当前用户借阅数据
    {
        Borrow* head = NULL;
        //读取借阅数据
        if (loadBorrows())
        {
            head = g_borrowHead;
        }
        else
        {
            fclose(fp);
            return;
        }
        Borrow* p = head;
        if (p == NULL)
        {
            showError("当前没有借阅数据！");
            fclose(fp);
            freeBorrowList(head);
            pauseScreen("按任意键继续…");
            return;
        }

        //写入表头
        fprintf(fp, "用户ID,ISBN编码,借阅时间,归还时间,是否归还(1-归还 0-未归还)\n");
        //遍历并筛选：仅当前用户的借阅记录
        while (p != NULL)
        {
            //匹配当前登录用户ID才写入
            if (strcmp(p->userId, g_currentUserId) == 0)
            {
                if (fprintf(fp, "%s,%s,%s,%s,%d\n",
                    p->userId, p->ISBN, p->borrowDate, p->returnDate, p->isReturn) < 0)
                {
                    setColor(12);
                    printf("借阅数据写入文件失败！错误原因：%s\n", strerror(errno));
                    setColor(15);
                    exportSuccess = 0;
                    pauseScreen("按任意键继续…");
                    break;
                }
            }
            p = p->next;
        }
        freeBorrowList(head);
    }
    //关闭文件
    fclose(fp);
    //导出结果提示
    if (exportSuccess)
    {
        showSuccess("数据导出成功！");
    }
    else
    {
        showError("数据导出失败！\n");
    }
    pauseScreen("按下任意键返回主菜单…");
    return;
}