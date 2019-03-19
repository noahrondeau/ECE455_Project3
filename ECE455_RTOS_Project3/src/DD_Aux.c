/*
 * DD_Aux.c
 *
 *  Created on: Mar 18, 2019
 *      Author: Matthew
 */


#include "DD_Aux.h"

void vPeriodicTask(void *pvParamaters){
	vTaskDelay(1000);
}

void vPeriodicGenerator(void *pvParamaters){

	DD_TaskHandle_t Generator = DD_TaskAlloc();
	u32 currentTick; //will most likely need a software timer to get sys time

	Generator->xTask = xPeriodicTask;
	Generator->xFunction = vPeriodicTask;
	Generator->xCreationTime = currentTick;
	Generator->xAbsDeadline = currentTick + 1000;
	Generator->xRelDeadline = 500;
	Generator->xPriority = DD_TASK_PRIORITY_UNRELEASED;
	Generator->xStatus = DD_TaskUninitialized;
	Generator->pNext = NULL;
	Generator->pPrev = NULL;

	while(!DD_Create(Generator));

	vTaskDelay(500);

}

