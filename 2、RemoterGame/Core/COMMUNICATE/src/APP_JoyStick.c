#include "APP_JoyStick.h"
#include "adc.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*
*********************************************************************************************************
*	��������DeadBand
*	����˵����ҡ����������
*	�βΣ�value:ҡ��ֵ
*			threshold:ҡ���м��������ֵ
*	����ֵ����
*********************************************************************************************************
*/
void DeadBand(int value, const int threshold)
{
	
}

/*
*********************************************************************************************************
*	��������JoyToDir
*	����˵����ҡ��ֵת��Ϊ����
*	�βΣ�joy:��ҡ�˻�����ҡ��
*	����ֵ������
*********************************************************************************************************
*/
Map_Status JoyToDir(LR_Joy joy)
{
	//ҡ�˵�ADֵ
	uint16_t AD_Left_Right = 0;
	uint16_t AD_Up_Down = 0;
	//��ȡ��/��ҡ��ADֵ
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

	//ADֵת��Ϊ����
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

