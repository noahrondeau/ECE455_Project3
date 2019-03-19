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
	ret->sTaskName = "";
	ret->uStackSize = configMINIMAL_STACK_SIZE;
	ret->xCreationTime = 0;
	ret->xAbsDeadline = 0;
	ret->xRelDeadline = 0;
	ret->xPriority = DD_TASK_PRIORITY_UNRELEASED;
	ret->xStatus = DD_TaskUninitialized;
	ret->pNext = NULL;
	ret->pPrev = NULL;

	return ret;
}

// deallocates a DD_TaskHandle_t pointer
// precondition:
//		ddTask was allocated using DD_TaskAlloc()
//		TaskHandle_t already independently deallocated
// 		by calling vTaskDelete on the internal ddTask->xTask
//		The next and prev pointers have been properly handled and set to null
DD_Status_t		DD_TaskDealloc(DD_TaskHandle_t ddTask)
{
	// xTask needs to be already null and properly deallocated: return error otherwise
	if (ddTask->xTask != NULL)
		return DD_Task_Dealloc_Fail_xTask_Not_Null;

	if (ddTask->pNext != NULL)
		return DD_Task_Dealloc_Fail_pNext_Not_Null;

	if (ddTask->pPrev != NULL)
		return DD_Task_Dealloc_Fail_pPrev_Not_Null;

	// other fields can be filled, xFunction is static
	// zero out the memory just as good practice
	ddTask->xFunction = NULL;
	ddTask->sTaskName = NULL;
	ddTask->uStackSize = 0;
	ddTask->xAbsDeadline = 0;
	ddTask->xCreationTime = 0;
	ddTask->xRelDeadline = 0;
	ddTask->xStatus = 0;

	vPortFree((void*)ddTask);

	return DD_Success;

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

	return DD_Success;
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
	return DD_None;
}

// traverse the list front the front, modifying deadlines until the removal point is found
DD_Status_t	DD_TaskListRemoveByHandle(DD_TaskListHandle_t list, DD_TaskHandle_t task)
{
	return DD_None;
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
	return DD_None;
}
