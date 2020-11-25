#include "APP_JoyStick.h"
#include "adc.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*
*********************************************************************************************************
*	函数名：DeadBand
*	功能说明：摇杆死区处理
*	形参：value:摇杆值
*			threshold:摇杆中间软件死区值
*	返回值：无
*********************************************************************************************************
*/
void DeadBand(int value, const int threshold)
{
	
}

/*
*********************************************************************************************************
*	函数名：JoyToDir
*	功能说明：摇杆值转换为方向
*	形参：joy:左摇杆或者右摇杆
*	返回值：方向
*********************************************************************************************************
*/
Map_Status JoyToDir(LR_Joy joy)
{
	//摇杆的AD值
	uint16_t AD_Left_Right = 0;
	uint16_t AD_Up_Down = 0;
	//读取左/右摇杆AD值
	if(joy == Left_Joy)
	{
		AD_Left_Right = getAdcValue(L_LEFT_RIGHT);
		AD_Up_Down = getAdcValue(L_UP_DOWN);
	}
	else if(joy == Right_Joy)
	{
		AD_Left_Right = getAdcValue(R_LEFT_RIGHT);
		AD_Up_Down = getAdcValue(R_UP_DOWN);
	}
	else
	{
		return Map_NULL;
	}

	//AD值转换为方向
	if(AD_Left_Right >= Right_Limit)
	{
		return Map_Right;
	}
	else if(AD_Left_Right <= Left_Limit)
	{
		return Map_Left;
	}
	else if(AD_Up_Down >= Up_Limit)
	{
		return Map_Up;
	}
	else if(AD_Up_Down <= Down_Limit)
	{
		return Map_Down;
	}
	
	return Map_NULL;
	
}

