#include "usart.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "oled.h"
#include "24l01.h"
#include "adc.h"
#include "bsp_key.h"

//#include "hw_config.h"
//#include "config_param.h"
//#include "beep.h"
//#include "radiolink.h"
//#include "usblink.h"
//#include "remoter_ctrl.h"
//#include "atkp.h"
#include "display.h"
#include "include.h"

//#include "keyTask.h"
#include "APP_Display.h"
/* FreeRtos includes */


#define BOOTLOADER_SIZE		(9*1024)		/*9K bootloader*/
#define CONFIG_PARAM_SIZE	(127*1024)		/*128K用于保存参数*/
	
#define CONFIG_PARAM_ADDR 	(FLASH_BASE + CONFIG_PARAM_SIZE)/*配置参数保存地址*/	
#define FIRMWARE_START_ADDR (FLASH_BASE + BOOTLOADER_SIZE)

static TaskHandle_t Task_DsiplayHandle;
static TaskHandle_t Task_LedHandle;
static TaskHandle_t Task_KeyHandle;
static TaskHandle_t Task_CreadFoodHandle;

EventGroupHandle_t SnakeEvent = NULL;
SemaphoreHandle_t Semaphore_Score = NULL;


void CreadTask(void);
void CreadVar(void);

int main(void)
{	
	NVIC_SetVectorTable(FIRMWARE_START_ADDR,0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);/*中断配置初始化*/
	delay_init(); 		/*delay初始化*/
	ledInit();			/*led初始化*/
	bsp_InitKey();
	oledInit();
	Adc_Init();
	
	CreadTask();	//创建任务
	CreadVar();		//创建标志组、信号量、定时器等
	vTaskStartScheduler();/*开启任务调度*/
	
	while(1){};/* 任务调度后不会执行到这 */
}

/*创建任务*/
void CreadTask(void)
{
	taskENTER_CRITICAL();	/*进入临界区*/
	
	xTaskCreate(Task_Dsiplay, 
						"Task_Dsiplay", 
						512, 
						NULL, 
						8, 
						&Task_DsiplayHandle);

	xTaskCreate(Task_Led, 
						"Task_Led", 
						64, 
						NULL, 
						5, 
						&Task_LedHandle);
	xTaskCreate(Task_Key, 
						"Task_Key", 
						256, 
						NULL, 
						6, 
						&Task_KeyHandle);

	xTaskCreate(Task_CreadFood, 
				"Task_CreadFood", 
				512, 
				NULL, 
				3, 
				&Task_CreadFoodHandle);
						
	taskEXIT_CRITICAL();	/*退出临界区*/
}

void CreadVar(void)
{
	taskENTER_CRITICAL();	/*进入临界区*/

	//创建蛇状态标志组
	SnakeEvent = xEventGroupCreate();
	xEventGroupClearBits(SnakeEvent, GameStatusStart|GameStatusPause|GameStatusWin|GameStatusLose|GameNoteFood|GameAddScore);
	xEventGroupSetBits(SnakeEvent, GameStatusStop);

	//创建计数器计分
	Semaphore_Score = xSemaphoreCreateCounting(Max_Score, 0);
	
	taskEXIT_CRITICAL();	/*退出临界区*/
}
