#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "library.h"

/*************************************************
  Function:countBooks
  Description:有关图书的各种统计
  Calls:centerPrint,freeBookList,loadBooks
       ,pauseScreen,showError
  Called By:statisticsMenu
  Input:无
  Output:馆藏书籍总数,已借出图书总数,当前在馆图书数 表格
  Return:无
  Others:无
*************************************************/
void countBooks()
{
	centerPrint(30, "图书数据统计");
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
	//判断头指针是否为空
	if (head == NULL)
	{
		showError("当前没有图书！");
	}
	else
	{
		//遍历链表得到数据
		int totalBooks = 0;
		int totalBorrowedBooks = 0;
		int totalExistingBooks = 0;
		Book* p = head;
		while (p != NULL)
		{
			totalBooks += (p->inventory + p->borrowedCount);
			totalBorrowedBooks += p->borrowedCount;
			totalExistingBooks += p->inventory;
			p = p->next;
		}
		//打印数据
		printf("┌──────────────┬─────────────┐\n");
		printf("│%14s│%12s│\n", "   数据类型   ", "     数量    ");
		printf("├──────────────┼─────────────┤\n");
		printf("│%14s│%10d %s│\n", "馆藏书籍总数", totalBooks, "本");
		printf("├──────────────┼─────────────┤\n");
		printf("│%14s│%10d %s│\n", "已借出图书总数", totalBorrowedBooks, "本");
		printf("├──────────────┼─────────────┤\n");
		printf("│%14s│%10d %s│\n", "当前在馆图书数", totalExistingBooks, "本");
		printf("└──────────────┴─────────────┘\n");
	}
	//释放链表
	freeBookList(head);
	pauseScreen("按下任意键返回上级菜单…");
}

/*************************************************
  Function:cmpBookBorrowedTimes
  Description:按照借阅次数降序排序的函数
  Calls:无
  Called By:maxBorrowedTime
  Others:maxBorrowedTime的qsort中使用的
*************************************************/
static int cmpBookBorrowedTimes(const void* a, const void* b)
{
	//传过来的指针是void类型，但原本是Book**类型的数组元素地址
	//所有先强制类型转换为Book**再解引用得到数组元素对应的Book*指针
	Book* book1 = *(Book**)a;
	Book* book2 = *(Book**)b;
	//需要降序，则 后-前
	return (book2->borrowTimes - book1->borrowTimes);
}

/*************************************************
  Function:maxBorrowedTime
  Description:借阅次数前10的书籍
  Calls:centerPrint,countGbkDot,freeBookList
       ,loadBooks,pauseScreen,showError
  Called By:main
  Input:无
  Output:借阅次数前10的书籍列表
  Return:无
  Others:将链表转为数组用qsort排序
*************************************************/
void maxBorrowedTime()
{
	centerPrint(166, "借阅书籍排行榜");
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
	//头指针是否为空
	if (head == NULL)
	{
		showError("当前没有图书！");
	}
	else
	{
		//统计链表节点数
		int bookNumber = 0;
		Book* p = head;
		while (p != NULL)
		{
			bookNumber++;
			p = p->next;
		}
		//将链表转为数组
		Book** bookArray = (Book**)malloc(bookNumber * sizeof(Book*));
		if (bookArray == NULL) 
		{
			showError("排序数组内存分配失败！");
			pauseScreen("按下任意键返回上级菜单…");
			return;
		}
		p = head;
		for (int i = 0; i < bookNumber && p != NULL; i++)
		{
			bookArray[i] = p;
			p = p->next;
		}
		//按降序排序
		qsort(bookArray, bookNumber, sizeof(Book*), cmpBookBorrowedTimes);
		//输出前10个（若不足10输出全部）
		int topCount = (bookNumber < 10) ? bookNumber : 10;
		printf("┌────┬──────────────┬──────────────────────────────────────────────────┬─────────────────────────────────┬──────────────────────────────────────────────────┬────────┐\n");
		printf("│%4s│%-14s│%-50s│%-33s│%-50s│%-8s│\n"
			,"序号", "ISBN", "书名", "作者", "出版社", "借阅次数");
		printf("├────┼──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
		for (int i = 0; i < topCount; i++)
		{
			//调用封装的函数统计书名和作者中·的个数
			int booknameDot = countGbkDot(bookArray[i]->bookName);
			int authorDot = countGbkDot(bookArray[i]->author);
			int totalDot = booknameDot + authorDot;
			//打印内容+补空格
			printf("│%4d│%-14s│%-50s│%-33s",i+1, bookArray[i]->ISBN
				, bookArray[i]->bookName, bookArray[i]->author);
			for (int i = 0; i < totalDot; i++)
			{
				printf(" ");
			}
			printf("│%-50s│%8d│\n"
				, bookArray[i]->publisher, bookArray[i]->borrowTimes);
			if (i != topCount - 1)
			{
				printf("├────┼──────────────┼──────────────────────────────────────────────────┼─────────────────────────────────┼──────────────────────────────────────────────────┼────────┤\n");
			}
		}
		printf("└────┴──────────────┴──────────────────────────────────────────────────┴─────────────────────────────────┴──────────────────────────────────────────────────┴────────┘\n");

	}
	//释放链表
	freeBookList(head);
	pauseScreen("按下任意键返回上级菜单…");
}

typedef struct UserFine 
{
	char userId[UID_LEN];  // 用户ID
	int totalFine;         // 该用户总罚金
} UserFine;
/*************************************************
  Function:countUserTotalFine
  Description:统计所有逾期用户的所有罚金
  Calls:getCurrentDateStr,isBookOverDue,loadBorrows
       ,pauseScreen,showError
  Called By:maxFineUsers
  Input:fine_count-总罚金人数的指针
  Output:无
  Return:UserFine类型的数组指针
  Others:无
*************************************************/
static UserFine* countUserTotalFine(int* fineCount)
{
	*fineCount = 0;
	UserFine* fineArray = NULL;
	//读取借阅数据
	Borrow* head = NULL;
	if (loadBorrows())
	{
		head = g_borrowHead;
	}
	else
	{
		return NULL;
	}
	Borrow* p = head;
	//判断头指针状态
	if (head == NULL)
	{
		showError("当前没有借阅记录！");
		pauseScreen("按下任意键返回上级菜单…");
		return NULL;
	}
	//获取当前时间字符串
	char currentDate[DATE_LEN] = "";
	if (!getCurrentDateStr(currentDate))
	{
		return NULL;
	}
	//遍历链表统计
	while (p != NULL)
	{
		int interval = 0;
		//没有归还并且逾期的才计算罚金
		if (p->isReturn == 0 && isBookOverDue(p->borrowDate,&interval) == 1)
		{
			//计算该条记录的罚金
			int fine = interval * g_singleDayFine;
			//汇总到对应用户（去重累加）
			int found = 0;//是否找到对应用户的标记
			//遍历当前记录的所有数组成员对应的结构体
			for (int i = 0; i < *fineCount; i++) 
			{
				if (strcmp(fineArray[i].userId, p->userId) == 0) 
				{
					fineArray[i].totalFine += fine; // 同一用户，累加罚金
					found = 1;//找到则标记为1
					break;
				}
			}
			//新用户：扩容数组并添加记录
			if (!found) 
			{
				//总用户数+1
				*fineCount += 1;
				//调整内存大小
				UserFine* new_fineArray = (UserFine*)realloc(fineArray, *fineCount * sizeof(UserFine));
				if(new_fineArray != NULL)
				{
					fineArray = new_fineArray;
					//拷贝用户信息+本条数据的罚金
					strncpy(fineArray[*fineCount - 1].userId, p->userId, UID_LEN - 1);
					fineArray[*fineCount - 1].userId[UID_LEN - 1] = '\0';
					fineArray[*fineCount - 1].totalFine = fine;
				}
				else
				{
					showError("调整数组内存大小失败！");
					pauseScreen("按下任意键返回上级菜单…");
					return NULL;
				}
			}
		}
		p = p->next;
	}
	return fineArray;
}

/*************************************************
  Function:cmpUserFine
  Description:按总罚金降序排列
  Calls:无
  Called By:maxFineUsers
  Others:maxFineUsers中qsort排序的比较函数
*************************************************/
static int cmpUserFine(const void* a, const void* b) 
{
	UserFine* uf1 = (UserFine*)a;
	UserFine* uf2 = (UserFine*)b;
	// 降序：后减前
	return (uf2->totalFine - uf1->totalFine); 
}

/*************************************************
  Function:maxFineUsers
  Description:展示罚金前5的用户排行榜
  Calls:centerPrint,countUserTotalFine,loadUsers
       ,pauseScreen,showError
  Called By:statisticsMenu
  Input:无
  Output:罚金前5的用户以表格展示
  Return:无
  Others:使用qsort排序
*************************************************/
void maxFineUsers()
{
	//输出功能标题
	centerPrint(79, "罚金排行榜");
	//读取用户数据
	User* userhead = NULL;
	if (loadUsers())
	{
		userhead = g_userHead;
	}
	else
	{
		return;
	}
	//统计所有用户的罚金
	int totalFineUsers = 0;
	UserFine* fineArray = countUserTotalFine(&totalFineUsers);
	if (fineArray == NULL)
	{
		return;
	}
	else if(totalFineUsers == 0)
	{
		showError("暂无逾期罚金记录！\n");
		pauseScreen("按下任意键返回上级菜单…");
		return;
	}
	//降序排序
	qsort(fineArray, totalFineUsers, sizeof(UserFine), cmpUserFine);
	//输出前5（不足5则输出全部）
	int topCount = (totalFineUsers < 5) ? totalFineUsers : 5;
	printf("┌────┬──────────┬──────────────────────────────────────────────────┬──────────┐\n");
	printf("│%4s│%-10s│%-50s│%10s│\n"
		, "序号", "用户ID", "姓名", "罚金总数");
	printf("├────┼──────────┼──────────────────────────────────────────────────┼──────────┤\n");
	for (int i = 0; i < topCount; i++)
	{
		printf("│%4d│%-10s│", i + 1, fineArray[i].userId);
		User* p = userhead;
		while (p != NULL)
		{
			if (strcmp(fineArray[i].userId, p->userId) == 0)
			{
				break;
			}
			p = p->next;
		}
		if(p != NULL)
		{
			printf("%-50s│%8d元│\n"
				, p->userName, fineArray[i].totalFine);
		}
		if (i != topCount - 1)
		{
			printf("├────┼──────────┼──────────────────────────────────────────────────┼──────────┤\n");
		}
	}
	printf("└────┴──────────┴──────────────────────────────────────────────────┴──────────┘\n");
	free(fineArray);
	pauseScreen("按下任意键返回上级菜单…");
	return;
}