/*
 * DD_Scheduler.h
 *
 *  Created on: Mar 14, 2019
 *      Author: ntron
 */

#ifndef DD_SCHEDULER_H_
#define DD_SCHEDULER_H_

#include "config.h"


/* ------------- Public Structs ----------------- */

typedef u32 DD_Status_t; //TODO: actually figure out what to return
typedef u32 LinkedList; //TODO: actually implement a list

typedef struct DD_TaskParams_t
{
	//TODO
} DD_TaskParams_t;


/* ------------- Public Functions --------------- */

TaskHandle_t	DD_TaskCreate(DD_TaskParams_t tparams);
DD_Status_t 	DD_TaskDelete(TaskHandle_t xTask);
DD_Status_t		DD_ReturnActiveList(LinkedList* retActiveList);
DD_Status_t		DD_ReturnOverdueList(LinkedList* retOverdueList);



#endif /* DD_SCHEDULER_H_ */
