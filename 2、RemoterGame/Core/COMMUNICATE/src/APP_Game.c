#include "oled.h"

#include "gui_basic.h"
#include "APP_Game.h"
#include "Text.h"

#include "include.h"



Map_Status GameMap[WallStopX+1][WallStopY+1];	//保存地图像素点的状态 上/下/左/右/空白/食物/墙壁


static uint8_t Cread_Snake(PointXY point, uint8_t length);
void Draw_Map_Wall(PointXY StartPoint, PointXY StopPoint, uint8_t Lintwith);

/*
*********************************************************************************************************
*	函数名：Set_Map_Val
*	功能说明：设置地图像素点状态
*	形参：x：像素点x坐标
*			y：像素点y坐标
*			Value:状态值
*	返回值：无
*********************************************************************************************************
*/
void Set_Map_Val(uint32_t x, uint32_t y, Map_Status Value)
{
	GameMap[x][y] = Value;
}

/*
*********************************************************************************************************
*	函数名：Set_Map_Val
*	功能说明：获取地图像素点状态
*	形参：x：像素点x坐标
*			y：像素点y坐标
*	返回值：地图像素点的状态
*********************************************************************************************************
*/
Map_Status Get_Map_Val(uint32_t x, uint32_t y)
{
	return GameMap[x][y];
}

/*
*********************************************************************************************************
*	函数名：Game_Init
*	功能说明：游戏初始化
*	形参：无
*	返回值：0:初始化成功 其他:失败
*********************************************************************************************************
*/
uint8_t Game_Init(void)
{
	uint8_t Err;
	PointXY WallStartP;	//墙壁起始坐标
	PointXY WallStopP;	//墙壁结束坐标
	PointXY InitSnake;
	uint8_t SnakeLength;
	uint8_t i,j=0;

	//初始化参数
	WallStartP.x = WallStartX;
	WallStartP.y = WallStartY;
	WallStopP.x = WallStopX;
	WallStopP.y = WallStopY;
	InitSnake.x = Snake_Init_PointX;
	InitSnake.y = Snake_Init_PointY;
	SnakeLength = Snake_Length;

	//清除显示
	oled_clear();

	//设置地图坐标状态为空
	for(i=WallStartP.x;i<WallStopP.x;i++)
	{
		for(j=WallStartP.y;j<WallStopP.y;j++)
			Set_Map_Val(i, j, Map_NULL);
	}
	
	Draw_Map_Wall(WallStartP, WallStopP, 1);
	Err = Cread_Snake(InitSnake, Snake_Length);
	oled_refreshGram(); 	//刷新屏幕

	return Err;
	
}

/*
*********************************************************************************************************
*	函数名：Draw_Map_Wall
*	功能说明：设置墙壁
*	形参：StartPoint：左上角起始点
*			StopPoint:右下角结束点
*			Lintwith:线宽
*	返回值：无
*********************************************************************************************************
*/
void Draw_Map_Wall(PointXY StartPoint, PointXY StopPoint, uint8_t Lintwith)
{
	uint8_t i,j=0;
	
	//画四条线
	GUI_Line(StartPoint.x, StartPoint.y, StopPoint.x, StartPoint.y, Color_White);	
	GUI_Line(StopPoint.x, StartPoint.y, StopPoint.x, StopPoint.y, Color_White);
	GUI_Line(StartPoint.x, StartPoint.y, StartPoint.x, StopPoint.y, Color_White);
	GUI_Line(StartPoint.x, StopPoint.y, StopPoint.x, StopPoint.y, Color_White);

	//设置墙壁
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
*	函数名：Cread_Snake
*	功能说明：创建一条x方向蛇,蛇头在右边
*	形参：x:蛇头x坐标
*			y:蛇头y坐标
*			length:蛇身长度
*	返回值：0:成功 其他:失败
*********************************************************************************************************
*/
static uint8_t Cread_Snake(PointXY point, uint8_t length)
{	
	uint8_t i=0;
	uint8_t j=0;
	//x方向在墙壁内2格,y方向在墙壁内才能创建成功
	if(point.x>=(WallStopX-(Snake_Step*2)))	//头超过墙壁内壁两格
		return 1;
	else if((point.x-length)<=(WallStartX+(Snake_Step*2)))	//尾超过墙壁内壁两格
		return 2;
	else if(point.y<=WallStartY)
		return 3;
	else if(point.y>=WallStopY)
		return 4;

	//创建蛇
	GUI_RectangleFill(point.x-(length*Snake_Add_Size)+1, point.y, point.x, point.y+Snake_Add_Size-1, Color_White);
	for(i=point.x-(length*Snake_Add_Size-1);i<=point.x;i++)
	{
		for(j=point.y;j<=point.y+Snake_Add_Size-1;j++)
			Set_Map_Val(i, j, Map_Right);	//默认向右走
	}
	

	
	return 0;
}

/*
*********************************************************************************************************
*	函数名：Snake_Run
*	功能说明：蛇运动
*	形参：HeadPoint:蛇头坐标
*			TailPoint:蛇尾坐标
*			Status:运动状态
*	返回值：0:成功 其他:失败
*********************************************************************************************************
*/
Game_Status Snake_Run(PointXY *HeadPoint, PointXY *TailPoint,Map_Status RunDir)
{
	int32_t i,j;
	
	PointXY NewHeadPoint;		//新的蛇头坐标
	PointXY NewTailPoint;		//新的蛇尾坐标
	
	Map_Status HeadStatus;		//获取蛇头的状态
	Map_Status TailStatus;		//获取蛇尾的状态
	Map_Status BfMapStatus = Map_NULL;		//获取蛇前面地图的状态
	Map_Status NewHeadStatus;	//新蛇头状态
	Map_Status NewTailStatus;	//新蛇尾状态
	uint8_t Refre = 0;		//更新蛇标志 bit0:更新蛇头 bit1:更新蛇尾 
	Game_Status GameStatus;	//游戏状态
	uint8_t FoodNum = 0;	//食物点阵数量
	uint8_t Score = 0;		//分数
	
	//设置新蛇头和蛇尾
	NewHeadPoint.x = HeadPoint->x;
	NewHeadPoint.y = HeadPoint->y;
	NewTailPoint.x = TailPoint->x;
	NewTailPoint.y = TailPoint->y;
	
	//有运动方向
	if(RunDir != Map_NULL)
	{
		//获取当前蛇头蛇尾的状态
		HeadStatus = Get_Map_Val(HeadPoint->x, HeadPoint->y);
		TailStatus = Get_Map_Val(TailPoint->x, TailPoint->y);

		Refre = 0;
		FoodNum = 0;
	}
	  
	switch(RunDir)
	{
		case Map_Up:{
						//旧蛇头不与当前方向相反
						if(HeadStatus != Map_Down)
						{
							//蛇头位置更新
							NewHeadPoint.y-=Snake_Add_Size;
							NewHeadStatus = Map_Up;
							HeadStatus = Map_Up;
							Refre |= 0x01;
						}
					}
			break;
		case Map_Down:{
						//旧蛇头不与当前方向相反
						if(HeadStatus != Map_Up)
						{
							//蛇头位置更新
							NewHeadPoint.y+=Snake_Add_Size;
							NewHeadStatus = Map_Down;
							HeadStatus = Map_Down;
							Refre |= 0x01;
							
						}
					}
			break;
		case Map_Left:{
						//旧蛇头不与当前方向相反
						if(HeadStatus != Map_Right)
						{
							//蛇头位置更新
							NewHeadPoint.x-=Snake_Add_Size;
							NewHeadStatus = Map_Left;
							HeadStatus = Map_Left;
							Refre |= 0x01;
						}
					}
			break;
		case Map_Right:{
						//旧蛇头不与当前方向相反
						if(HeadStatus != Map_Left)
						{
							//蛇头位置更新
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

	//更新蛇头
	if((Refre&0x01) == 0x01)
	{
		//超出屏幕
		if(((NewHeadPoint.x-Snake_Add_Size+1)<0)||(NewHeadPoint.y<0)||(NewHeadPoint.x>127)||(NewHeadPoint.y>63))
		{
			xEventGroupSetBits(SnakeEvent, GameStatusLose);	//游戏结束
			xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusRun|GameStatusPause);	//游戏结束
			NewHeadPoint.x = HeadPoint->x;
			NewHeadPoint.y = HeadPoint->y;
	
			return GameStatus;
		}
		//判断前面地图状况
		for(i=(NewHeadPoint.x-Snake_Add_Size+1);i<=NewHeadPoint.x;i++)
		{
			for(j=NewHeadPoint.y;j<=(NewHeadPoint.y+Snake_Add_Size-1);j++)
			{
				BfMapStatus = Get_Map_Val(i, j);
				
				if((BfMapStatus != Map_Food)&&(BfMapStatus != Map_NULL))	//前面是墙壁或者身体
				{
					xEventGroupSetBits(SnakeEvent, GameStatusLose);	//游戏结束
					xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusRun|GameStatusPause);	//游戏结束
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
		
		//前面是食物
		if(FoodNum==(Snake_Add_Size*Snake_Add_Size))
		{
			//分数+1
			xSemaphoreGive(Semaphore_Score);
			Score = uxSemaphoreGetCount(Semaphore_Score);	//读取分数
			Score*=Food_Score;
			//显示分数
			oled_showNum(105, 26, Score, 3, 6, 12);
			//赢了
			if(Score >= Max_Score)
			{
				Score = 0;
				xEventGroupSetBits(SnakeEvent, GameStatusWin);	//游戏结束
				xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusRun|GameStatusPause);	//游戏结束
			}
			xEventGroupSetBits(SnakeEvent, GameNoteFood);	//食物为空
		}
		else
		{
			Refre |= 0x02;
		}
		
		
		//设置头坐标状态
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

		GUI_RectangleFill(NewHeadPoint.x-Snake_Add_Size+1, NewHeadPoint.y, NewHeadPoint.x, NewHeadPoint.y+Snake_Add_Size-1, 1);	//画蛇头

		//设置成新蛇头
		HeadPoint->x = NewHeadPoint.x;
		HeadPoint->y = NewHeadPoint.y;
	}
	//更新蛇尾
	if((Refre&0x02) == 0x02)
	{
		//读取蛇尾的状态
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
		
		
		//设置尾坐标状态
		for(i=(TailPoint->x-Snake_Add_Size+1);i<=TailPoint->x;i++)
		{
			for(j=TailPoint->y;j<=(TailPoint->y+Snake_Add_Size-1);j++)
				Set_Map_Val(i, j, TailStatus);
		}
		
		GUI_RectangleFill(TailPoint->x-Snake_Add_Size+1, TailPoint->y, TailPoint->x, TailPoint->y+Snake_Add_Size-1, 0);	//清蛇尾
	
		TailPoint->x = NewTailPoint.x;
		TailPoint->y = NewTailPoint.y;
	}


	return GameStatus;
}

