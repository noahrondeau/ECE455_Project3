/*
 * DD_Scheduler.h
 *
 *  Created on: Mar 14, 2019
 *      Author: ntron
 */

#ifndef DD_SCHEDULER_H_
#define DD_SCHEDULER_H_

#include "config.h"
#include "DDTask.h"


/* ------------- Public Structs ----------------- */

typedef u32 DDStatus_t; //TODO: actually figure out what to return


/* ------------- Public Functions --------------- */

TaskHandle_t	DD_TaskCreate(DDTask_t tparams);
DDStatus_t 		DD_TaskDelete(DDTask_t xTask);
DDStatus_t		DD_ReturnActiveList(DDTaskList_t* retActiveList);
DDStatus_t		DD_ReturnOverdueList(DDTaskList_t* retOverdueList);



#endif /* DD_SCHEDULER_H_ */
