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
DD_Status_t	DD_TaskListInsertByDeadline(DD_TaskListHandle_t list, DD_TaskHandle_t ddTask)
{
	// case list is empty
	if (DD_TaskListIsEmpty(list))
	{
		list->pHead = ddTask;
		list->pTail = ddTask;
		list->uSize = 1;
		ddTask->xPriority = DD_TASK_USER_PRIORITY(0);
		vTaskPrioritySet(ddTask->xTask, ddTask->xPriority);
		return DD_Success;
	}

	// FALL THROUGH CASE: NOT EMPTY

	// traverse starting at the head of the list
	// increase priorities while we aren't there
	DD_TaskHandle_t pAux = list->pHead; // auxilliary pointer for traversing list

	while(pAux != NULL) // traverse until the end
	{

		if (ddTask->xAbsDeadline < pAux->xAbsDeadline)
		{
			// insert the piece here, being careful if its at the front or the back

			// if at the front
			ddTask->pNext = pAux;
			ddTask->pPrev = pAux->pPrev; // will be null if pAux == pHead
			pAux->pPrev = ddTask;

			// if inserting at the front, move the head pointer
			if (pAux == list->pHead )
				list->pHead = ddTask;

			// set the priority to 1 more than pAux priority
			ddTask->xPriority = pAux->xPriority  + 1;
			vTaskPrioritySet(ddTask->xTask, ddTask->xPriority);

			(list->uSize)++;
			return DD_Success; // early return from here

		}
		else
		{
			// if the deadline of the new task is greater OR EQUAL to a task,
			// increase the priority of the task then move to the next one
			// theres a special case here if we are inspecting the last item currently in the list
			pAux->xPriority += 1;
			vTaskPrioritySet(pAux->xTask, pAux->xPriority);

			if ( pAux == list->pTail )
			{
				//if last node, insert after
				ddTask->pPrev = pAux;
				ddTask->pNext = NULL;
				pAux->pNext = ddTask;
				list->pTail = ddTask;

				ddTask->xPriority = DD_TASK_USER_PRIORITY(0);
				vTaskPrioritySet(ddTask->xTask, ddTask->xPriority);
			}

			pAux++; // no early return, want to keep iterating
					// or, if at end of list, then we just inserted,
					// and we can safely fall out of the loop and return

		}
	}

	(list->uSize)++;
	return DD_Success;
}

// traverse the list front the front, modifying deadlines until the removal point is found
DD_Status_t	DD_TaskListRemoveByHandle(DD_TaskListHandle_t list, DD_TaskHandle_t ddTask)
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
