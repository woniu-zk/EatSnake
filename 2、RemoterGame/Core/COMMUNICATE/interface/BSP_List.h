#ifndef _BSP_LIST_H
#define _BSP_LIST_H

#include "gui_basic.h"

//链表结构体
typedef struct _Node
{
	PointXY value;
	struct _Node *next;
}Node;

typedef struct 
{
	Node *Head;	//表头
	Node *Tail;	//表尾
}List;


void List_Add(List     *list, PointXY value);
void List_Delet(List *list, PointXY value);
uint8_t List_Insert(List *list, PointXY value, int8_t i);
uint8_t List_Length(List *list);
uint8_t List_Read_Value(List *list, uint8_t i, PointXY *value);


#endif

