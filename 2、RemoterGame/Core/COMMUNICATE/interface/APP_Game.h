#ifndef _GAME_H
#define _GAME_H

#include "gui_basic.h"
#include "APP_JoyStick.h"

//墙壁
#define WallStartX 0
#define WallStartY 0
#define WallStopX 101
#define WallStopY 61

//蛇初始化状态
#define Snake_Init_PointX 60
#define Snake_Init_PointY 30
#define Snake_Length 3	//蛇初始长度

#define Snake_Step 5	//每步的像素点

#define Food_Size 5	//食物大小
#define Snake_Add_Size 5 //蛇每一块的大小
//分数
#define Food_Score 5	//每块食物分数
#define Max_Score  100	//最大分数



#define GameStatusStart		(0x00000001<<0)		//游戏开始  
#define GameStatusPause		(0x00000001<<1)  	//游戏暂停
#define GameStatusStop		(0x00000001<<2)  	//游戏停止
#define GameAddScore		(0x00000001<<3)  	//游戏增加蛇头
#define GameStatusWin		(0x00000001<<4)  	//游戏赢了
#define GameStatusLose		(0x00000001<<5)  	//游戏输了
#define GameNoteFood		(0x00000001<<6) 	//游戏没有食物
#define GameStatusRun		(0x00000001<<7)		//游戏运行 





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

