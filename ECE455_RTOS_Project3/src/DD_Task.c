/*
 * DDTask.c
 *
 *  Created on: Mar 17, 2019
 *      Author: ntron
 */

#include <DD_Task.h>


/*------------------- Task Functions ----------------*/

// malloc a DD_Task_t and return handle
// leaves all fields at default values and internal pointers null
DD_TaskHandle_t DD_TaskAlloc()
{
	DD_TaskHandle_t ret = (DD_TaskHandle_t)pvPortMalloc(sizeof(DD_Task_t));

	ret->xTask = NULL;
	ret->xFunction = NULL;
	ret->xCreationTime = 0;
	ret->xAbsDeadline = 0;
	ret->xRelDeadline = 0;
	ret->xPriority = DD_TASK_PRIORITY_UNRELEASED;
	ret->xStatus = DD_TaskUninitialized;
	ret->pNext = NULL;
	ret->pPrev = NULL;

	return ret;
}

/*----------------- Task List Functions -------------
 * This is an ordered linked list
 * The API ensures that anything inserted remains sorted by ascending deadline
 * (the front of the list has the earliest deadlines, and ergo highest priorities)
 * The API takes care of modifying priorities upon insertion and removal from the list
 */

DD_Status_t DD_TaskListInit(DD_TaskListHandle_t list)
{
	list->uSize = 0;
	list->pHead = NULL;
	list->pTail = NULL;

	return 0;
}

u32	DD_TaskListGetSize(DD_TaskListHandle_t list)
{
	return list->uSize;
}

bool DD_TaskListIsEmpty(DD_TaskListHandle_t list)
{
	return (list->uSize == 0);
}

// traverse the list from the front, modifying deadlines until the insertion point is found
DD_Status_t	DD_TaskListInsertByDeadline(DD_TaskListHandle_t list, DD_TaskHandle_t task)
{
	return 0;
}

// traverse the list front the front, modifying deadlines until the removal point is found
DD_Status_t	DD_TaskListRemoveByHandle(DD_TaskListHandle_t list, DD_TaskHandle_t task)
{
	return 0;
}

// collect all overdue tasks into a temp list and return it (by value for the list struct)
DD_TaskList_t DD_TaskListRemoveOverdue(DD_TaskListHandle_t list, TickType_t currentTime)
{
	DD_TaskList_t ret;
	DD_TaskListInit(&ret);
	return ret;
}

// append list2 to list1
DD_Status_t	DD_TaskListConcatenate(DD_TaskListHandle_t list1, DD_TaskListHandle_t list2)
{
	return 0;
}
