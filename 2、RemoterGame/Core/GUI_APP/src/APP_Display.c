#include "APP_Display.h"
#include "led.h"
#include "oled.h"
#include "gui_basic.h"
#include "adc.h"
#include "BSP_List.h"
#include "bsp_key.h"
#include "APP_Game.h"

#include "APP_JoyStick.h"
#include "include.h"
#include "stdlib.h"

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern Map_Status JoyToDir(LR_Joy joy);


List List_Block_XY; //方块xy位置链表

/*
*********************************************************************************************************
*	函数名：Joy_Control_Block
*	功能说明：摇杆控制方块移动
*	形参：LR_JOY：左摇杆或者右摇杆控制
*	返回值：无
*********************************************************************************************************
*/
//void Joy_Control_Block(LR_Joy LR_JOY)
//{
//	//方块基础坐标
//	volatile static PointXY Start_Point = {.x = Block_Point_X, .y = Block_Point_Y};
//	volatile static PointXY Stop_Point = {.x = (Block_Point_X + Block_Length), .y = (Block_Point_Y + Block_Wide)};
//	
//	Dir Right_Dir = JOY_NOT;
//	
//	//读取摇杆动作
//	Right_Dir = JoyToDir(LR_JOY);

//	//摇杆有动作
//	if(Right_Dir != JOY_NOT)
//	{
//		GUI_RectangleFill(Start_Point.x, Start_Point.y, Stop_Point.x, Stop_Point.y, 0);	//删除方块
//	}
//	  
//	switch(Right_Dir)
//	{
//		case Joy_UP:{
//						Start_Point.y-= Joy_Step_Move;
//						Stop_Point.y = Start_Point.y + Block_Wide;
//					}
//			break;
//		case Joy_DOWN:{
//						Start_Point.y+= Joy_Step_Move;
//						Stop_Point.y = Start_Point.y + Block_Wide;
//					}
//			break;
//		case Joy_LEFT:{
//						Start_Point.x-= Joy_Step_Move;
//						Stop_Point.x = Start_Point.x + Block_Length;
//					}
//			break;
//		case Joy_RIGHT:{
//						Start_Point.x+= Joy_Step_Move;
//						Stop_Point.x = Start_Point.x + Block_Length;
//					}
//			break;
//		default:
//			break;
//	}

//	//摇杆有动作
//	if(Right_Dir != JOY_NOT)
//	{
//		//左右边界
//		if((int8_t)Start_Point.x < X_LLimit)
//		{
//			Start_Point.x = X_LLimit;
//			Stop_Point.x = Start_Point.x + Block_Length;
//		}
//		else if(Stop_Point.x > X_HLimit)
//		{
//			Stop_Point.x = X_HLimit;
//			Start_Point.x = Stop_Point.x - Block_Length;
//		}

//		//上下边界
//		if((int8_t)Start_Point.y < Y_LLimit)
//		{
//			Start_Point.y = Y_LLimit;
//			Stop_Point.y = Start_Point.y + Block_Wide;
//		}
//		else if(Stop_Point.y > Y_HLimit)
//		{
//			Stop_Point.y = Y_HLimit;
//			Start_Point.y = Stop_Point.y - Block_Wide;
//		}
//		GUI_RectangleFill(Start_Point.x, Start_Point.y, Stop_Point.x, Stop_Point.y, 1);	//画方块
//		//List_Add(&List_Block_XY, Start_Point);	//加入到链表中
//	}
//}


/*
*********************************************************************************************************
*	函数名：Task_Led
*	功能说明：LED任务
*	形参：*param
*	返回值：无
*********************************************************************************************************
*/
void Task_Led(void *param)
{
	TickType_t Increment = 500;
	TickType_t pxPreviousWakeTime = 0;
	pxPreviousWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		LED_BLUE = !LED_BLUE;
		vTaskDelayUntil(&pxPreviousWakeTime, Increment);
	}
}

/*
*********************************************************************************************************
*	函数名：Task_CreadFood
*	功能说明：产生食物
*	形参：*param
*	返回值：无
*********************************************************************************************************
*/
void Task_CreadFood(void *param)
{
	uint32_t result = 0;
	TickType_t Increment = 100;
	TickType_t pxPreviousWakeTime = 0;
	TickType_t RandSeed = 0;
	PointXY FoodPoint;	//食物位置
	uint8_t i,j;
	uint8_t StatusCreadFood = 0;	//创造食物标志位 0:不能创造 1:能创造
	
	FoodPoint.x = 0;
	FoodPoint.y = 0;
	pxPreviousWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		//等待没有食物再产生食物
		result = xEventGroupWaitBits(SnakeEvent, GameNoteFood, pdTRUE, pdFAIL, portMAX_DELAY);

		if((result&GameNoteFood) == GameNoteFood)
		{
			RandSeed = xTaskGetTickCount();

			FoodPoint.x = (int32_t)((RandSeed%19+1)*Snake_Add_Size);
			FoodPoint.y = (int32_t)((RandSeed%11+1)*Snake_Add_Size);
			StatusCreadFood = 1;

			//判断食物所在位置为空
			for(i=(FoodPoint.x-Snake_Add_Size+1);i<=FoodPoint.x;i++)
			{
				for(j=FoodPoint.y;j<(FoodPoint.y+Snake_Add_Size);j++)
				{
					//该片区域不为空
					if(Get_Map_Val(FoodPoint.x, FoodPoint.y) != Map_NULL)
					{
						xEventGroupSetBits(SnakeEvent, GameNoteFood);	//食物为空
						StatusCreadFood = 0;
					}
						
				}
			}
			if(StatusCreadFood)
			{
				//创建食物
				for(i=(FoodPoint.x-Snake_Add_Size+1);i<=FoodPoint.x;i++)
				{
					for(j=FoodPoint.y;j<(FoodPoint.y+Snake_Add_Size);j++)
					{
						Set_Map_Val(i, j, Map_Food);
						
					}
				}
				xEventGroupClearBits(SnakeEvent, GameNoteFood);	//食物不为空
				GUI_RectangleFill(FoodPoint.x-Snake_Add_Size+1, FoodPoint.y, FoodPoint.x, FoodPoint.y+Snake_Add_Size-1, 1);	//画方块
			}
		}

		vTaskDelayUntil(&pxPreviousWakeTime, Increment);
	}
}

/*
*********************************************************************************************************
*	函数名：Task_Key
*	功能说明：按键扫描任务
*	形参：*param
*	返回值：无
*********************************************************************************************************
*/
void Task_Key(void *param)
{
	TickType_t Increment = 10;
	TickType_t pxPreviousWakeTime = 0;
	pxPreviousWakeTime = xTaskGetTickCount();
	uint32_t result = 0;

	
	while(1)
	{     
		bsp_KeyScan();

		
		//左边按键按下
		if(bsp_GetKey()==KEY_DOWN_L)
		{
			result = xEventGroupGetBits(SnakeEvent);
			//游戏运行中按下KEY_L游戏暂停
			if((result&GameStatusRun) == GameStatusRun)
			{
				xEventGroupSetBits(SnakeEvent, GameStatusPause);	//暂停游戏
				xEventGroupClearBits(SnakeEvent, GameStatusRun|GameStatusStart|GameStatusStop);
			}
			else if((result&GameStatusPause) == GameStatusPause)	//游戏暂停中按下KEY_L游戏继续
			{
				xEventGroupSetBits(SnakeEvent, GameStatusRun);	//继续游戏
				xEventGroupClearBits(SnakeEvent, GameStatusPause|GameStatusStop|GameStatusStart);
			}
			else if((result&GameStatusStop) == GameStatusStop)	//游戏停止中按下KEY_L重现开始
			{
				xEventGroupSetBits(SnakeEvent, GameStatusStart);	//开始游戏
				xEventGroupClearBits(SnakeEvent, GameStatusPause|GameStatusStop|GameStatusRun);
			}
		}
			
		vTaskDelayUntil(&pxPreviousWakeTime, Increment);
	
	}
}


/*
*********************************************************************************************************
*	函数名：Task_Dsiplay
*	功能说明：显示任务
*	形参：*param
*	返回值：无
*********************************************************************************************************
*/
void Task_Dsiplay(void *param)
{
	TickType_t Increment = 100;
	TickType_t pxPreviousWakeTime = 0;
	pxPreviousWakeTime = xTaskGetTickCount();
	uint8_t n = 0;
	uint8_t List_Len = 0;	//链表长度
	PointXY ReDisplayXY;	//重现xy坐标
	PointXY LastReDisplayXY;	//上次重现xy坐标	

	Map_Status Right_Dir = Map_NULL;
	
	PointXY HeadPoint;
	PointXY TailPoint;

	uint8_t NotKeyCnt = 0;	//没有按键次数
	uint32_t result = 0;
	HeadPoint.x = Snake_Init_PointX;
	HeadPoint.y = Snake_Init_PointY;
	TailPoint.x = Snake_Init_PointX - ((Snake_Length-1)*Snake_Add_Size);
	TailPoint.y = Snake_Init_PointY;
	
	while(1)
	{
		
		//左边按键没有按下移动方块
		//if(bsp_GetKey()!=KEY_DOWN_L)
		{
			
			//读取摇杆动作
			Right_Dir = JoyToDir(Right_Joy);
			//没有按键则延续蛇头动作
			if(Right_Dir==Map_NULL)
			{
				NotKeyCnt++;
				if(NotKeyCnt==5)
				{
					NotKeyCnt = 0;
					Right_Dir = Get_Map_Val(HeadPoint.x, HeadPoint.y);
				}
			}

			result = xEventGroupGetBits(SnakeEvent);
			
			if((result&GameStatusStop) == GameStatusStop)		//游戏停止
			{
				oled_showString(5, 30, "Key L Start Game", 6, 12);
			}
			else if((result&GameStatusPause) == GameStatusPause)	//游戏暂停
			{
				
			}
			else if((result&GameStatusWin) == GameStatusWin)	//游戏赢了
			{
				//清除显示
				oled_clear();
				oled_showString(0, 0, "YOU WIN!", 12, 24);
				oled_refreshGram();		//刷新屏幕
				xEventGroupClearBits(SnakeEvent, GameStatusWin);	//游戏运行
				xEventGroupSetBits(SnakeEvent, GameStatusStop);	//游戏运行
			}
			else if((result&GameStatusLose) == GameStatusLose)	//游戏输了
			{
				//清除显示
				oled_clear();
				oled_showString(0, 0, "GAME OVER", 12, 24);
				oled_refreshGram();		//刷新屏幕
				xEventGroupClearBits(SnakeEvent, GameStatusLose);
				xEventGroupSetBits(SnakeEvent, GameStatusStop);	//游戏运行
				
			}
			else if((result&GameStatusStart) == GameStatusStart)	//游戏开始
			{
				//游戏初始化
				Game_Init();
				//设置蛇初始位置
				HeadPoint.x = Snake_Init_PointX;
				HeadPoint.y = Snake_Init_PointY;
				TailPoint.x = Snake_Init_PointX - ((Snake_Length-1)*Snake_Add_Size);
				TailPoint.y = Snake_Init_PointY;
				oled_showString(102, 0, "sc", 6, 12);
				//初始化计数器
				vSemaphoreDelete(Semaphore_Score);
				Semaphore_Score = xSemaphoreCreateCounting(Max_Score, 0);
				xEventGroupSetBits(SnakeEvent, GameNoteFood);	//食物为空
				xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusLose|GameStatusWin|GameStatusPause|GameStatusStop);	//清除状态
				xEventGroupSetBits(SnakeEvent, GameStatusRun);	//游戏运行
				
			}
			else if((result&GameStatusRun) == GameStatusRun)	//游戏运行
			{
				Snake_Run(&HeadPoint, &TailPoint, Right_Dir);
			}
			
			oled_refreshGram();		//刷新屏幕
		}
		//else	//左边按键按下复现方块
		{
//			oled_clear();//清屏
//			List_Len = List_Length(&List_Block_XY);
//			for(n = 1;n<(List_Len+1);n++)
//			{
//				GUI_RectangleFill(LastReDisplayXY.x, LastReDisplayXY.y, LastReDisplayXY.x+Block_Length, LastReDisplayXY.y+Block_Wide, 0);	//删除方块
//				List_Read_Value(&List_Block_XY, n, &ReDisplayXY);
//				GUI_RectangleFill(ReDisplayXY.x, ReDisplayXY.y, ReDisplayXY.x+Block_Length, ReDisplayXY.y+Block_Wide, 1);	//画方块
//				LastReDisplayXY.x = ReDisplayXY.x;
//				LastReDisplayXY.y = ReDisplayXY.y;
//				oled_refreshGram();		//刷新屏幕
//				vTaskDelayUntil(&pxPreviousWakeTime, Increment);
//			}
		}
		vTaskDelayUntil(&pxPreviousWakeTime, Increment);
	}
}
