#ifndef _GAME_H
#define _GAME_H

#include "gui_basic.h"
#include "APP_JoyStick.h"

//ǽ��
#define WallStartX 0
#define WallStartY 0
#define WallStopX 101
#define WallStopY 61

//�߳�ʼ��״̬
#define Snake_Init_PointX 60
#define Snake_Init_PointY 30
#define Snake_Length 3	//�߳�ʼ����

#define Snake_Step 5	//ÿ�������ص�

#define Food_Size 5	//ʳ���С
#define Snake_Add_Size 5 //��ÿһ��Ĵ�С
//����
#define Food_Score 5	//ÿ��ʳ�����
#define Max_Score  100	//������



#define GameStatusStart		(0x00000001<<0)		//��Ϸ��ʼ  
#define GameStatusPause		(0x00000001<<1)  	//��Ϸ��ͣ
#define GameStatusStop		(0x00000001<<2)  	//��Ϸֹͣ
#define GameAddScore		(0x00000001<<3)  	//��Ϸ������ͷ
#define GameStatusWin		(0x00000001<<4)  	//��ϷӮ��
#define GameStatusLose		(0x00000001<<5)  	//��Ϸ����
#define GameNoteFood		(0x00000001<<6) 	//��Ϸû��ʳ��
#define GameStatusRun		(0x00000001<<7)		//��Ϸ���� 





typedef enum
{
	Map_NULL = 0,
	Map_Up,
	Map_Down,
	Map_Left,
	Map_Right,
	Map_Wall,
	Map_Food,
}Map_Status;

typedef enum
{
	Game_Start = 0,
	Game_Pause,
	Game_Stop,
	Game_AddScore,
	Game_Win,
	Game_Lose,
}Game_Status;



void Draw_Map_Wall(PointXY StartPoint, PointXY StopPoint, uint8_t Lintwith);
uint8_t Game_Init(void);
void Set_Map_Val(uint32_t x, uint32_t y, Map_Status Value);
Map_Status Get_Map_Val(uint32_t x, uint32_t y);
Game_Status Snake_Run(PointXY *HeadPoint, PointXY *TailPoint,Map_Status RunDir);


#endif

