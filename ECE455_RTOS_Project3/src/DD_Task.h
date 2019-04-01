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
#include "DD_Status.h"

typedef enum DD_TaskStatus_t
{
	DD_TaskUninitialized,
	DD_TaskActive,
	DD_TaskDeleted,
	DD_TaskOverdue,
} DD_TaskStatus_t;

typedef enum DD_TaskType_t
{
	DD_TaskUnclassified,
	DD_TaskPeriodic,
	DD_TaskSporadic,
} DD_TaskType_t;


typedef struct DD_Task_t
{
	TaskHandle_t 		xTask;
	DD_TaskType_t		xTaskType;
	TaskFunction_t 		xFunction;
	TimerHandle_t		xTimer;
	TickType_t			xCreationTime;
	TickType_t			xRelDeadline;
	TickType_t			xAbsDeadline;
	DD_TaskPriority_t	xPriority;
	DD_TaskStatus_t		xStatus;
	char*				sTaskName;
	u16					uStackSize;
	struct DD_Task_t*	pPrev;
	struct DD_Task_t*	pNext;

} DD_Task_t;

typedef DD_Task_t* DD_TaskHandle_t;

typedef struct DD_TaskList_t
{
	u32					uSize;
	DD_TaskHandle_t		pHead;
	DD_TaskHandle_t		pTail;
} DD_TaskList_t;

typedef DD_TaskList_t* DD_TaskListHandle_t;

/*------------------- Task Functions ----------------*/

// malloc a DD_Task_t and return handle
// leaves all fields at default values and internal pointers null
DD_TaskHandle_t DD_TaskAlloc();

// deallocates a DD_TaskHandle_t pointer
// precondition:
//		ddTask was allocated using DD_TaskAlloc()
//		TaskHandle_t already independently deallocated
// 		by calling vTaskDelete on the internal ddTask->xTask
//
DD_Status_t		DD_TaskDealloc(DD_TaskHandle_t ddTask);

/*----------------- Task List Functions -------------
 * This is an ordered linked list
 */

DD_Status_t 			DD_TaskListInit(DD_TaskListHandle_t list);
u32						DD_TaskListGetSize(DD_TaskListHandle_t list);
bool					DD_TaskListIsEmpty(DD_TaskListHandle_t list);
DD_Status_t				DD_TaskListInsertByDeadline(DD_TaskListHandle_t list, DD_TaskHandle_t ddTask);
DD_Status_t				DD_TaskListRemoveByHandle(DD_TaskListHandle_t list, DD_TaskHandle_t ddTask);
DD_Status_t				DD_TaskListRemoveOverdue(DD_TaskListHandle_t active, DD_TaskListHandle_t overdue, TickType_t currentTime);
char*					DD_TaskListDataReturn(DD_TaskListHandle_t list);


#endif /* DD_TASK_H_ */
