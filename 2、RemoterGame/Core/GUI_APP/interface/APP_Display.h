#ifndef _APP_DISPLAY_H
#define _APP_DISPLAY_H

#include "APP_JoyStick.h"

//显示块的初始值
#define Block_Point_X 60
#define Block_Point_Y 30
#define Block_Length 10
#define Block_Wide 10

#define Joy_Step_Move	5	//摇杆每次动作块移动

//x、y边界
#define X_LLimit	0
#define X_HLimit	127
#define Y_LLimit	0
#define Y_HLimit	63


void Joy_Control_Block(LR_Joy LR_JOY);
void Task_Led(void *param);
void Task_CreadFood(void *param);
void Task_Dsiplay(void *param);
void Task_Key(void *param);

#endif

