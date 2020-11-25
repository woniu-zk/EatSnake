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


List List_Block_XY; //����xyλ������

/*
*********************************************************************************************************
*	��������Joy_Control_Block
*	����˵����ҡ�˿��Ʒ����ƶ�
*	�βΣ�LR_JOY����ҡ�˻�����ҡ�˿���
*	����ֵ����
*********************************************************************************************************
*/
//void Joy_Control_Block(LR_Joy LR_JOY)
//{
//	//�����������
//	volatile static PointXY Start_Point = {.x = Block_Point_X, .y = Block_Point_Y};
//	volatile static PointXY Stop_Point = {.x = (Block_Point_X + Block_Length), .y = (Block_Point_Y + Block_Wide)};
//	
//	Dir Right_Dir = JOY_NOT;
//	
//	//��ȡҡ�˶���
//	Right_Dir = JoyToDir(LR_JOY);

//	//ҡ���ж���
//	if(Right_Dir != JOY_NOT)
//	{
//		GUI_RectangleFill(Start_Point.x, Start_Point.y, Stop_Point.x, Stop_Point.y, 0);	//ɾ������
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

//	//ҡ���ж���
//	if(Right_Dir != JOY_NOT)
//	{
//		//���ұ߽�
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

//		//���±߽�
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
//		GUI_RectangleFill(Start_Point.x, Start_Point.y, Stop_Point.x, Stop_Point.y, 1);	//������
//		//List_Add(&List_Block_XY, Start_Point);	//���뵽������
//	}
//}


/*
*********************************************************************************************************
*	��������Task_Led
*	����˵����LED����
*	�βΣ�*param
*	����ֵ����
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
*	��������Task_CreadFood
*	����˵��������ʳ��
*	�βΣ�*param
*	����ֵ����
*********************************************************************************************************
*/
void Task_CreadFood(void *param)
{
	uint32_t result = 0;
	TickType_t Increment = 100;
	TickType_t pxPreviousWakeTime = 0;
	TickType_t RandSeed = 0;
	PointXY FoodPoint;	//ʳ��λ��
	uint8_t i,j;
	uint8_t StatusCreadFood = 0;	//����ʳ���־λ 0:���ܴ��� 1:�ܴ���
	
	FoodPoint.x = 0;
	FoodPoint.y = 0;
	pxPreviousWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		//�ȴ�û��ʳ���ٲ���ʳ��
		result = xEventGroupWaitBits(SnakeEvent, GameNoteFood, pdTRUE, pdFAIL, portMAX_DELAY);

		if((result&GameNoteFood) == GameNoteFood)
		{
			RandSeed = xTaskGetTickCount();

			FoodPoint.x = (int32_t)((RandSeed%19+1)*Snake_Add_Size);
			FoodPoint.y = (int32_t)((RandSeed%11+1)*Snake_Add_Size);
			StatusCreadFood = 1;

			//�ж�ʳ������λ��Ϊ��
			for(i=(FoodPoint.x-Snake_Add_Size+1);i<=FoodPoint.x;i++)
			{
				for(j=FoodPoint.y;j<(FoodPoint.y+Snake_Add_Size);j++)
				{
					//��Ƭ����Ϊ��
					if(Get_Map_Val(FoodPoint.x, FoodPoint.y) != Map_NULL)
					{
						xEventGroupSetBits(SnakeEvent, GameNoteFood);	//ʳ��Ϊ��
						StatusCreadFood = 0;
					}
						
				}
			}
			if(StatusCreadFood)
			{
				//����ʳ��
				for(i=(FoodPoint.x-Snake_Add_Size+1);i<=FoodPoint.x;i++)
				{
					for(j=FoodPoint.y;j<(FoodPoint.y+Snake_Add_Size);j++)
					{
						Set_Map_Val(i, j, Map_Food);
						
					}
				}
				xEventGroupClearBits(SnakeEvent, GameNoteFood);	//ʳ�ﲻΪ��
				GUI_RectangleFill(FoodPoint.x-Snake_Add_Size+1, FoodPoint.y, FoodPoint.x, FoodPoint.y+Snake_Add_Size-1, 1);	//������
			}
		}

		vTaskDelayUntil(&pxPreviousWakeTime, Increment);
	}
}

/*
*********************************************************************************************************
*	��������Task_Key
*	����˵��������ɨ������
*	�βΣ�*param
*	����ֵ����
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

		
		//��߰�������
		if(bsp_GetKey()==KEY_DOWN_L)
		{
			result = xEventGroupGetBits(SnakeEvent);
			//��Ϸ�����а���KEY_L��Ϸ��ͣ
			if((result&GameStatusRun) == GameStatusRun)
			{
				xEventGroupSetBits(SnakeEvent, GameStatusPause);	//��ͣ��Ϸ
				xEventGroupClearBits(SnakeEvent, GameStatusRun|GameStatusStart|GameStatusStop);
			}
			else if((result&GameStatusPause) == GameStatusPause)	//��Ϸ��ͣ�а���KEY_L��Ϸ����
			{
				xEventGroupSetBits(SnakeEvent, GameStatusRun);	//������Ϸ
				xEventGroupClearBits(SnakeEvent, GameStatusPause|GameStatusStop|GameStatusStart);
			}
			else if((result&GameStatusStop) == GameStatusStop)	//��Ϸֹͣ�а���KEY_L���ֿ�ʼ
			{
				xEventGroupSetBits(SnakeEvent, GameStatusStart);	//��ʼ��Ϸ
				xEventGroupClearBits(SnakeEvent, GameStatusPause|GameStatusStop|GameStatusRun);
			}
		}
			
		vTaskDelayUntil(&pxPreviousWakeTime, Increment);
	
	}
}


/*
*********************************************************************************************************
*	��������Task_Dsiplay
*	����˵������ʾ����
*	�βΣ�*param
*	����ֵ����
*********************************************************************************************************
*/
void Task_Dsiplay(void *param)
{
	TickType_t Increment = 100;
	TickType_t pxPreviousWakeTime = 0;
	pxPreviousWakeTime = xTaskGetTickCount();
	uint8_t n = 0;
	uint8_t List_Len = 0;	//������
	PointXY ReDisplayXY;	//����xy����
	PointXY LastReDisplayXY;	//�ϴ�����xy����	

	Map_Status Right_Dir = Map_NULL;
	
	PointXY HeadPoint;
	PointXY TailPoint;

	uint8_t NotKeyCnt = 0;	//û�а�������
	uint32_t result = 0;
	HeadPoint.x = Snake_Init_PointX;
	HeadPoint.y = Snake_Init_PointY;
	TailPoint.x = Snake_Init_PointX - ((Snake_Length-1)*Snake_Add_Size);
	TailPoint.y = Snake_Init_PointY;
	
	while(1)
	{
		
		//��߰���û�а����ƶ�����
		//if(bsp_GetKey()!=KEY_DOWN_L)
		{
			
			//��ȡҡ�˶���
			Right_Dir = JoyToDir(Right_Joy);
			//û�а�����������ͷ����
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
			
			if((result&GameStatusStop) == GameStatusStop)		//��Ϸֹͣ
			{
				oled_showString(5, 30, "Key L Start Game", 6, 12);
			}
			else if((result&GameStatusPause) == GameStatusPause)	//��Ϸ��ͣ
			{
				
			}
			else if((result&GameStatusWin) == GameStatusWin)	//��ϷӮ��
			{
				//�����ʾ
				oled_clear();
				oled_showString(0, 0, "YOU WIN!", 12, 24);
				oled_refreshGram();		//ˢ����Ļ
				xEventGroupClearBits(SnakeEvent, GameStatusWin);	//��Ϸ����
				xEventGroupSetBits(SnakeEvent, GameStatusStop);	//��Ϸ����
			}
			else if((result&GameStatusLose) == GameStatusLose)	//��Ϸ����
			{
				//�����ʾ
				oled_clear();
				oled_showString(0, 0, "GAME OVER", 12, 24);
				oled_refreshGram();		//ˢ����Ļ
				xEventGroupClearBits(SnakeEvent, GameStatusLose);
				xEventGroupSetBits(SnakeEvent, GameStatusStop);	//��Ϸ����
				
			}
			else if((result&GameStatusStart) == GameStatusStart)	//��Ϸ��ʼ
			{
				//��Ϸ��ʼ��
				Game_Init();
				//�����߳�ʼλ��
				HeadPoint.x = Snake_Init_PointX;
				HeadPoint.y = Snake_Init_PointY;
				TailPoint.x = Snake_Init_PointX - ((Snake_Length-1)*Snake_Add_Size);
				TailPoint.y = Snake_Init_PointY;
				oled_showString(102, 0, "sc", 6, 12);
				//��ʼ��������
				vSemaphoreDelete(Semaphore_Score);
				Semaphore_Score = xSemaphoreCreateCounting(Max_Score, 0);
				xEventGroupSetBits(SnakeEvent, GameNoteFood);	//ʳ��Ϊ��
				xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusLose|GameStatusWin|GameStatusPause|GameStatusStop);	//���״̬
				xEventGroupSetBits(SnakeEvent, GameStatusRun);	//��Ϸ����
				
			}
			else if((result&GameStatusRun) == GameStatusRun)	//��Ϸ����
			{
				Snake_Run(&HeadPoint, &TailPoint, Right_Dir);
			}
			
			oled_refreshGram();		//ˢ����Ļ
		}
		//else	//��߰������¸��ַ���
		{
//			oled_clear();//����
//			List_Len = List_Length(&List_Block_XY);
//			for(n = 1;n<(List_Len+1);n++)
//			{
//				GUI_RectangleFill(LastReDisplayXY.x, LastReDisplayXY.y, LastReDisplayXY.x+Block_Length, LastReDisplayXY.y+Block_Wide, 0);	//ɾ������
//				List_Read_Value(&List_Block_XY, n, &ReDisplayXY);
//				GUI_RectangleFill(ReDisplayXY.x, ReDisplayXY.y, ReDisplayXY.x+Block_Length, ReDisplayXY.y+Block_Wide, 1);	//������
//				LastReDisplayXY.x = ReDisplayXY.x;
//				LastReDisplayXY.y = ReDisplayXY.y;
//				oled_refreshGram();		//ˢ����Ļ
//				vTaskDelayUntil(&pxPreviousWakeTime, Increment);
//			}
		}
		vTaskDelayUntil(&pxPreviousWakeTime, Increment);
	}
}
