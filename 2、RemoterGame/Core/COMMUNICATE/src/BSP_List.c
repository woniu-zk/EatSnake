#include "BSP_List.h"
#include "stdlib.h"
/*
*********************************************************************************************************
*	函数名：List_Add
*	功能说明：给链表增加一个节点
*	形参：*list:指向链表的指针
*			value:节点对应的数据
*	返回值：无
*********************************************************************************************************
*/
void List_Add(List     *list, PointXY value)
{
	Node *p;
	
	p = (Node *)malloc(sizeof(Node));	
	
	p->value.x = value.x;
	p->value.y = value.y;
	p->next = NULL;
		
	//表头为空
	if(list->Head == NULL)
	{
		list->Head = p;
		list->Tail = p;
	}
	else
	{
		list->Tail->next = p; 
		list->Tail = p;
	}
}

/*
*********************************************************************************************************
*	函数名：List_Delet
*	功能说明：删除链表中某一数值对应的节点
*	形参：*list:指向链表的指针
*			value:节点对应的数据
*	返回值：无
*********************************************************************************************************
*/
void List_Delet(List *list, PointXY value)
{
	Node *p = list->Head;	//从头开始遍历
	Node *Last_p =list->Head;

	//判断当前节点数据与需要删除的数据是否一致
	while(((p->value.x != value.x)||(p->value.y != value.y))&&(p != list->Tail))
	{
		Last_p = p;			//保存当前节点地址
		p = p->next;	//移动到下一个节点地址
	}
	
	//判断是否为表头 
	if(p == list->Head)
	{
		//下一个节点为表头 
		list->Head = p->next;
	}
	//判断是否是最后一个节点 
	else if(p == list->Tail)
	{
		//上一节点为最后一个节点
		Last_p->next = NULL;	
		list->Tail = Last_p; 
	}
	else
	{
		Last_p ->next = p->next;	//把上一节点的指针指向下一个节点的地址
	}

	Last_p ->next = p->next;	//把上一节点的指针指向下一个节点的地址

	p ->value.x = 0;
	p ->value.y = 0;
	
	free(p);	//释放本次节点的空间
	p = NULL;
}

/*
*********************************************************************************************************
*	函数名：List_Insert
*	功能说明：插入一个节点到链表中某个位置
*	形参：*list:指向链表的指针
*			value:节点对应的数据
*			i:插入的位置
*	返回值：无
*********************************************************************************************************
*/
uint8_t List_Insert(List *list, PointXY value, int8_t i)
{
	uint8_t n = 0;
	Node *p;
	Node *s;
	
	//位置检查
	if(i<1)
	{
		return 0;
	}

	
	p = (Node *)malloc(sizeof(Node));
	p->value.x = value.x;
	p->value.y = value.y;
	
	s = list->Head;
	
	//遍历
	for(n=1; n<i; n++)
	{
		s = s->next;
		//超出当前链表的范围
		if(s->next == NULL)
		{
			if(n != (i-1))
			{
				free(p);
				p = NULL;
				return 0;
			}
			else
			{
				list->Tail = p;
			}
		}
	}
	p->next = s->next;
	s->next = p;
	
	return 1;
}

/*
*********************************************************************************************************
*	函数名：List_Length
*	功能说明：获取线性表的长度
*	形参：*list:指向链表的指针
*	返回值：线性表的长度
*********************************************************************************************************
*/
uint8_t List_Length(List *list)
{
	uint8_t n = 1;
	Node *p;
	p = list->Head;

	//一直遍历到链表尾
	while(p->next!=NULL)
	{
		p = p->next;
		n++;
	}
	return n;
}

/*
*********************************************************************************************************
*	函数名：List_Read_Value
*	功能说明：从链表中读取一个数据
*	形参：*list:指向链表的指针
*			i:读取数据的位置
*			value:获取的数据值
*	返回值：0:错误 1:正确
*********************************************************************************************************
*/
uint8_t List_Read_Value(List *list, uint8_t i, PointXY *value)
{
	Node *p = list->Head;	//从头开始遍历

	//判断当前节点位置与需要读取的位置是否一致
	while(--i)
	{
		p = p->next;	//移动到下一个节点地址
		//超出链表位置
		if((p->next == NULL)&&(i>1))
		{
			return 0;
		}
	}
	value->x = p->value.x;
	value->y = p->value.y;
	return 1;
}

