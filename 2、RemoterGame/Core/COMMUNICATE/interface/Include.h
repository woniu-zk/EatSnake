#ifndef _INCLUDE_H
#define _INCLUDE_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"

extern EventGroupHandle_t SnakeEvent;
extern SemaphoreHandle_t Semaphore_Score;

#endif
