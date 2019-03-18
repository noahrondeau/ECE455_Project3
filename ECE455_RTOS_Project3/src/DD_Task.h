/*
 * DDTask.h
 *
 *  Created on: Mar 17, 2019
 *      Author: ntron
 */

#ifndef DD_TASK_H_
#define DD_TASK_H_

#include "config.h"
#include "DD_TaskPriority.h"

typedef enum DD_TaskStatus_t
{
	DD_TaskUninitialized,
	DD_TaskActive,
	DD_TaskOverdue,
} DD_TaskStatus_t;


typedef struct DD_Task_t
{
	TaskHandle_t 		xTask;
	TaskFunction_t 		xFunction;
	TickType_t			xCreationTime;
	TickType_t			xRelDeadline;
	TickType_t			xAbsDeadline;
	DD_TaskPriority_t	xTaskPriority;
	DD_TaskStatus_t		xStatus;
	struct DD_Task_t*	pPrev;
	struct DD_Task_t*	pNext;

} DD_Task_t;

typedef struct DD_TaskList_t
{
	u32				uSize;
	DD_Task_t*		pListHead;
	DD_Task_t*		pListTail;
} DD_TaskList_t;



#endif /* DD_TASK_H_ */
