/*
 * DD_Aux.c
 *
 *  Created on: Mar 18, 2019
 *      Author: Matthew
 */


#include "DD_Aux.h"

void vPeriodicTask(void *pvParamaters){
	vTaskDelay(1000);
	//Not sure how to actually do the self delete
	//Might have to use pvParams with that being the struct for the particular task?
	//DD_TaskDelete();
}

void vPeriodicGenerator(void *pvParamaters){

	DD_TaskHandle_t Generator = DD_TaskAlloc();
	u32 currentTick; //will most likely need a software timer to get sys time

	//refers to the empty periodic task
	Generator->xTask = xPeriodicTask;
	Generator->xFunction = vPeriodicTask;

	//subject to change
	/*Desired:
	 * Get the current system time from either a timer or some other way in ticks or s
	 * Adjusts abs and rel deadlines based on making it periodic and current sys time
	 */
	Generator->xCreationTime = currentTick;
	Generator->xAbsDeadline = currentTick + 1000;
	Generator->xRelDeadline = 500;

	//Leave as an initialized struct and let DD_Scheduler change these*/
	Generator->xPriority = DD_TASK_PRIORITY_UNRELEASED;
	Generator->xStatus = DD_TaskUninitialized;
	Generator->pNext = NULL;
	Generator->pPrev = NULL;

	DD_TaskCreate(Generator);

	//Delay as per the example in the lab manual, we can change this to whatever we want
	vTaskDelay(250);

}

