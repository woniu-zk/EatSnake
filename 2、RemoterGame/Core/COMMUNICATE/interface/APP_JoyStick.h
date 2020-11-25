#ifndef _APP_JOYSTICK_H
#define _APP_JOYSTICK_H
#include "APP_Game.h"

//摇杆AD值转换为方向的数值
#define Up_Limit 3000
#define Down_Limit 1000
#define Left_Limit 1000
#define Right_Limit 3000

typedef enum
{
	Left_Joy = 0x00,
	Right_Joy = 0x10,
}LR_Joy;


//方向
typedef enum
{
	JOY_NOT = 0,
	Joy_UP,
	Joy_DOWN,
	Joy_LEFT,
	Joy_RIGHT,
	
}Dir;

//AD通道x对应的摇杆方向
typedef enum
{
	R_LEFT_RIGHT = 1,
	R_UP_DOWN,
	L_LEFT_RIGHT,
	L_UP_DOWN,
}Ad_Channl;

void DeadBand(int value, const int threshold);


#endif


