/*
 * DD_Scheduler.h
 *
 *  Created on: Mar 14, 2019
 *      Author: ntron
 */

#ifndef DD_SCHEDULER_H_
#define DD_SCHEDULER_H_

#include <DD_Task.h>
#include "config.h"


/* ------------- Public Structs ----------------- */

typedef u32 DD_Status_t; //TODO: actually figure out what to return


/* ------------- Public Functions --------------- */

TaskHandle_t	DD_TaskCreate(DD_Task_t tparams);
DD_Status_t 	DD_TaskDelete(DD_Task_t xTask);
DD_Status_t		DD_ReturnActiveList(DD_TaskList_t* retActiveList);
DD_Status_t		DD_ReturnOverdueList(DD_TaskList_t* retOverdueList);



#endif /* DD_SCHEDULER_H_ */
