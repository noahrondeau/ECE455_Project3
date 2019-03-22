/*
 * DD_Scheduler.h
 *
 *  Created on: Mar 14, 2019
 *      Author: ntron
 */

#ifndef DD_SCHEDULER_H_
#define DD_SCHEDULER_H_

#include "config.h"
#include "DD_Task.h"
#include "DD_Status.h"


/* ------------- Definitions ------------------- */

#define SCHEDULER_MAX_USER_TASKS_NUM		(20)


/* ------------- Public Functions --------------- */

DD_Status_t		DD_SchedulerStart(void);

DD_Status_t		DD_TaskCreate(DD_TaskHandle_t ddTask);
DD_Status_t 	DD_TaskDelete(DD_TaskHandle_t ddTask); // could take a DD_TaskHandle_t obtained with container_of
DD_Status_t		DD_ReturnActiveList(DD_TaskListHandle_t retActiveList);
DD_Status_t		DD_ReturnOverdueList(DD_TaskListHandle_t retOverdueList);



#endif /* DD_SCHEDULER_H_ */
