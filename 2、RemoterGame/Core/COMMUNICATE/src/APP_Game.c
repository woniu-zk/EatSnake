#include "oled.h"

#include "gui_basic.h"
#include "APP_Game.h"
#include "Text.h"

#include "include.h"



Map_Status GameMap[WallStopX+1][WallStopY+1];	//�����ͼ���ص��״̬ ��/��/��/��/�հ�/ʳ��/ǽ��


static uint8_t Cread_Snake(PointXY point, uint8_t length);
void Draw_Map_Wall(PointXY StartPoint, PointXY StopPoint, uint8_t Lintwith);

/*
*********************************************************************************************************
*	��������Set_Map_Val
*	����˵�������õ�ͼ���ص�״̬
*	�βΣ�x�����ص�x����
*			y�����ص�y����
*			Value:״ֵ̬
*	����ֵ����
*********************************************************************************************************
*/
void Set_Map_Val(uint32_t x, uint32_t y, Map_Status Value)
{
	GameMap[x][y] = Value;
}

/*
*********************************************************************************************************
*	��������Set_Map_Val
*	����˵������ȡ��ͼ���ص�״̬
*	�βΣ�x�����ص�x����
*			y�����ص�y����
*	����ֵ����ͼ���ص��״̬
*********************************************************************************************************
*/
Map_Status Get_Map_Val(uint32_t x, uint32_t y)
{
	return GameMap[x][y];
}

/*
*********************************************************************************************************
*	��������Game_Init
*	����˵������Ϸ��ʼ��
*	�βΣ���
*	����ֵ��0:��ʼ���ɹ� ����:ʧ��
*********************************************************************************************************
*/
uint8_t Game_Init(void)
{
	uint8_t Err;
	PointXY WallStartP;	//ǽ����ʼ����
	PointXY WallStopP;	//ǽ�ڽ�������
	PointXY InitSnake;
	uint8_t SnakeLength;
	uint8_t i,j=0;

	//��ʼ������
	WallStartP.x = WallStartX;
	WallStartP.y = WallStartY;
	WallStopP.x = WallStopX;
	WallStopP.y = WallStopY;
	InitSnake.x = Snake_Init_PointX;
	InitSnake.y = Snake_Init_PointY;
	SnakeLength = Snake_Length;

	//�����ʾ
	oled_clear();

	//���õ�ͼ����״̬Ϊ��
	for(i=WallStartP.x;i<WallStopP.x;i++)
	{
		for(j=WallStartP.y;j<WallStopP.y;j++)
			Set_Map_Val(i, j, Map_NULL);
	}
	
	Draw_Map_Wall(WallStartP, WallStopP, 1);
	Err = Cread_Snake(InitSnake, Snake_Length);
	oled_refreshGram(); 	//ˢ����Ļ

	return Err;
	
}

/*
*********************************************************************************************************
*	��������Draw_Map_Wall
*	����˵��������ǽ��
*	�βΣ�StartPoint�����Ͻ���ʼ��
*			StopPoint:���½ǽ�����
*			Lintwith:�߿�
*	����ֵ����
*********************************************************************************************************
*/
void Draw_Map_Wall(PointXY StartPoint, PointXY StopPoint, uint8_t Lintwith)
{
	uint8_t i,j=0;
	
	//��������
	GUI_Line(StartPoint.x, StartPoint.y, StopPoint.x, StartPoint.y, Color_White);	
	GUI_Line(StopPoint.x, StartPoint.y, StopPoint.x, StopPoint.y, Color_White);
	GUI_Line(StartPoint.x, StartPoint.y, StartPoint.x, StopPoint.y, Color_White);
	GUI_Line(StartPoint.x, StopPoint.y, StopPoint.x, StopPoint.y, Color_White);

	//����ǽ��
	for(i=StartPoint.x;i<=StopPoint.x;i++)
		Set_Map_Val(i, StartPoint.y, Map_Wall);
	for(i=StartPoint.y;i<=StopPoint.y;i++)
		Set_Map_Val(StartPoint.x, i, Map_Wall);
	for(i=StartPoint.x;i<=StopPoint.x;i++)
		Set_Map_Val(i, StopPoint.y, Map_Wall);
	for(i=StartPoint.y;i<=StopPoint.y;i++)
		Set_Map_Val(StopPoint.x, i, Map_Wall);
}

/*
*********************************************************************************************************
*	��������Cread_Snake
*	����˵��������һ��x������,��ͷ���ұ�
*	�βΣ�x:��ͷx����
*			y:��ͷy����
*			length:������
*	����ֵ��0:�ɹ� ����:ʧ��
*********************************************************************************************************
*/
static uint8_t Cread_Snake(PointXY point, uint8_t length)
{	
	uint8_t i=0;
	uint8_t j=0;
	//x������ǽ����2��,y������ǽ���ڲ��ܴ����ɹ�
	if(point.x>=(WallStopX-(Snake_Step*2)))	//ͷ����ǽ���ڱ�����
		return 1;
	else if((point.x-length)<=(WallStartX+(Snake_Step*2)))	//β����ǽ���ڱ�����
		return 2;
	else if(point.y<=WallStartY)
		return 3;
	else if(point.y>=WallStopY)
		return 4;

	//������
	GUI_RectangleFill(point.x-(length*Snake_Add_Size)+1, point.y, point.x, point.y+Snake_Add_Size-1, Color_White);
	for(i=point.x-(length*Snake_Add_Size-1);i<=point.x;i++)
	{
		for(j=point.y;j<=point.y+Snake_Add_Size-1;j++)
			Set_Map_Val(i, j, Map_Right);	//Ĭ��������
	}
	

	
	return 0;
}

/*
*********************************************************************************************************
*	��������Snake_Run
*	����˵�������˶�
*	�βΣ�HeadPoint:��ͷ����
*			TailPoint:��β����
*			Status:�˶�״̬
*	����ֵ��0:�ɹ� ����:ʧ��
*********************************************************************************************************
*/
Game_Status Snake_Run(PointXY *HeadPoint, PointXY *TailPoint,Map_Status RunDir)
{
	int32_t i,j;
	
	PointXY NewHeadPoint;		//�µ���ͷ����
	PointXY NewTailPoint;		//�µ���β����
	
	Map_Status HeadStatus;		//��ȡ��ͷ��״̬
	Map_Status TailStatus;		//��ȡ��β��״̬
	Map_Status BfMapStatus = Map_NULL;		//��ȡ��ǰ���ͼ��״̬
	Map_Status NewHeadStatus;	//����ͷ״̬
	Map_Status NewTailStatus;	//����β״̬
	uint8_t Refre = 0;		//�����߱�־ bit0:������ͷ bit1:������β 
	Game_Status GameStatus;	//��Ϸ״̬
	uint8_t FoodNum = 0;	//ʳ���������
	uint8_t Score = 0;		//����
	
	//��������ͷ����β
	NewHeadPoint.x = HeadPoint->x;
	NewHeadPoint.y = HeadPoint->y;
	NewTailPoint.x = TailPoint->x;
	NewTailPoint.y = TailPoint->y;
	
	//���˶�����
	if(RunDir != Map_NULL)
	{
		//��ȡ��ǰ��ͷ��β��״̬
		HeadStatus = Get_Map_Val(HeadPoint->x, HeadPoint->y);
		TailStatus = Get_Map_Val(TailPoint->x, TailPoint->y);

		Refre = 0;
		FoodNum = 0;
	}
	  
	switch(RunDir)
	{
		case Map_Up:{
						//����ͷ���뵱ǰ�����෴
						if(HeadStatus != Map_Down)
						{
							//��ͷλ�ø���
							NewHeadPoint.y-=Snake_Add_Size;
							NewHeadStatus = Map_Up;
							HeadStatus = Map_Up;
							Refre |= 0x01;
						}
					}
			break;
		case Map_Down:{
						//����ͷ���뵱ǰ�����෴
						if(HeadStatus != Map_Up)
						{
							//��ͷλ�ø���
							NewHeadPoint.y+=Snake_Add_Size;
							NewHeadStatus = Map_Down;
							HeadStatus = Map_Down;
							Refre |= 0x01;
							
						}
					}
			break;
		case Map_Left:{
						//����ͷ���뵱ǰ�����෴
						if(HeadStatus != Map_Right)
						{
							//��ͷλ�ø���
							NewHeadPoint.x-=Snake_Add_Size;
							NewHeadStatus = Map_Left;
							HeadStatus = Map_Left;
							Refre |= 0x01;
						}
					}
			break;
		case Map_Right:{
						//����ͷ���뵱ǰ�����෴
						if(HeadStatus != Map_Left)
						{
							//��ͷλ�ø���
							NewHeadPoint.x+=Snake_Add_Size;
							NewHeadStatus = Map_Right;
							HeadStatus = Map_Right;
							Refre |= 0x01;
						}
					}
			break;
		default:
			break;
	}

	//������ͷ
	if((Refre&0x01) == 0x01)
	{
		//������Ļ
		if(((NewHeadPoint.x-Snake_Add_Size+1)<0)||(NewHeadPoint.y<0)||(NewHeadPoint.x>127)||(NewHeadPoint.y>63))
		{
			xEventGroupSetBits(SnakeEvent, GameStatusLose);	//��Ϸ����
			xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusRun|GameStatusPause);	//��Ϸ����
			NewHeadPoint.x = HeadPoint->x;
			NewHeadPoint.y = HeadPoint->y;
	
			return GameStatus;
		}
		//�ж�ǰ���ͼ״��
		for(i=(NewHeadPoint.x-Snake_Add_Size+1);i<=NewHeadPoint.x;i++)
		{
			for(j=NewHeadPoint.y;j<=(NewHeadPoint.y+Snake_Add_Size-1);j++)
			{
				BfMapStatus = Get_Map_Val(i, j);
				
				if((BfMapStatus != Map_Food)&&(BfMapStatus != Map_NULL))	//ǰ����ǽ�ڻ�������
				{
					xEventGroupSetBits(SnakeEvent, GameStatusLose);	//��Ϸ����
					xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusRun|GameStatusPause);	//��Ϸ����
					NewHeadPoint.x = HeadPoint->x;
					NewHeadPoint.y = HeadPoint->y;
			
					return GameStatus;
				}
				if(BfMapStatus == Map_Food)
				{
					FoodNum++;
				}
			}
		}
		
		//ǰ����ʳ��
		if(FoodNum==(Snake_Add_Size*Snake_Add_Size))
		{
			//����+1
			xSemaphoreGive(Semaphore_Score);
			Score = uxSemaphoreGetCount(Semaphore_Score);	//��ȡ����
			Score*=Food_Score;
			//��ʾ����
			oled_showNum(105, 26, Score, 3, 6, 12);
			//Ӯ��
			if(Score >= Max_Score)
			{
				Score = 0;
				xEventGroupSetBits(SnakeEvent, GameStatusWin);	//��Ϸ����
				xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusRun|GameStatusPause);	//��Ϸ����
			}
			xEventGroupSetBits(SnakeEvent, GameNoteFood);	//ʳ��Ϊ��
		}
		else
		{
			Refre |= 0x02;
		}
		
		
		//����ͷ����״̬
		for(i=(HeadPoint->x-Snake_Add_Size+1);i<=HeadPoint->x;i++)
		{
			for(j=HeadPoint->y;j<=(HeadPoint->y+Snake_Add_Size-1);j++)
				Set_Map_Val(i, j, HeadStatus);
		}
		for(i=(NewHeadPoint.x-Snake_Add_Size+1);i<=NewHeadPoint.x;i++)
		{
			for(j=NewHeadPoint.y;j<=(NewHeadPoint.y+Snake_Add_Size-1);j++)
				Set_Map_Val(i, j, NewHeadStatus);
		}

		GUI_RectangleFill(NewHeadPoint.x-Snake_Add_Size+1, NewHeadPoint.y, NewHeadPoint.x, NewHeadPoint.y+Snake_Add_Size-1, 1);	//����ͷ

		//���ó�����ͷ
		HeadPoint->x = NewHeadPoint.x;
		HeadPoint->y = NewHeadPoint.y;
	}
	//������β
	if((Refre&0x02) == 0x02)
	{
		//��ȡ��β��״̬
		switch(TailStatus)
		{
			case Map_Up:{
							NewTailPoint.y-=Snake_Add_Size;
						}
				break;
			case Map_Down:{
							NewTailPoint.y+=Snake_Add_Size;
						}
				
				break;
			case Map_Left:{
							NewTailPoint.x-=Snake_Add_Size;
						}
				break;
			case Map_Right:{
							NewTailPoint.x+=Snake_Add_Size;
						}
				break;
			default:
				break;
		}

		TailStatus = Map_NULL;
		
		
		//����β����״̬
		for(i=(TailPoint->x-Snake_Add_Size+1);i<=TailPoint->x;i++)
		{
			for(j=TailPoint->y;j<=(TailPoint->y+Snake_Add_Size-1);j++)
				Set_Map_Val(i, j, TailStatus);
		}
		
		GUI_RectangleFill(TailPoint->x-Snake_Add_Size+1, TailPoint->y, TailPoint->x, TailPoint->y+Snake_Add_Size-1, 0);	//����β
	
		TailPoint->x = NewTailPoint.x;
		TailPoint->y = NewTailPoint.y;
	}


	return GameStatus;
}

