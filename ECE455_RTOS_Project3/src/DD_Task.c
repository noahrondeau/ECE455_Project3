/*
 * DDTask.c
 *
 *  Created on: Mar 17, 2019
 *      Author: ntron
 */

#include <DD_Task.h>
#include "SafePrint.h"


/*------------------- Task Functions ----------------*/

// malloc a DD_Task_t and return handle
// leaves all fields at default values and internal pointers null
DD_TaskHandle_t DD_TaskAlloc()
{
	DD_TaskHandle_t ret = (DD_TaskHandle_t)pvPortMalloc(sizeof(DD_Task_t));

	ret->xTask = NULL;
	ret->xFunction = NULL;
	ret->xTimer = NULL;
	ret->sTaskName = "";
	ret->uStackSize = configMINIMAL_STACK_SIZE;
	ret->xCreationTime = 0;
	ret->xAbsDeadline = 0;
	ret->xRelDeadline = 0;
	ret->xPriority = DD_TASK_PRIORITY_UNRELEASED;
	ret->xStatus = DD_TaskUninitialized;
	ret->xTaskType = DD_TaskUnclassified;
	ret->pNext = NULL;
	ret->pPrev = NULL;

	return ret;
}

// deallocates a DD_TaskHandle_t pointer
// precondition:
//		ddTask was allocated using DD_TaskAlloc()
//		TaskHandle_t already independently deallocated !!!!!!!!!!!!!! NOT ACTUALLY SURE THIS MATTERS
// 		by calling vTaskDelete on the internal ddTask->xTask
//		The next and prev pointers have been properly handled and set to null
DD_Status_t		DD_TaskDealloc(DD_TaskHandle_t ddTask)
{
	// xTask needs to be already null and properly deallocated: return error otherwise
	//if (ddTask->xTask != NULL)
		//return DD_Task_Dealloc_Fail_xTask_Not_Null;

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
	ddTask->xStatus = DD_TaskUninitialized;
	ddTask->xTaskType = DD_TaskUnclassified;

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
	SafePrintFromTask(DEBUG_LIST, "List Size before insertion: %d\n", list->uSize);

	ddTask->xStatus = DD_TaskActive;

	// case list is empty
	if (DD_TaskListIsEmpty(list))
	{
		list->pHead = ddTask;
		list->pTail = ddTask;
		list->uSize = 1;
		ddTask->xPriority = DD_TASK_USER_PRIORITY(0);
		vTaskPrioritySet(ddTask->xTask, ddTask->xPriority);
		SafePrintFromTask(DEBUG_LIST, "List Size after insertion: %d\n", list->uSize);
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

			if (pAux->pPrev != NULL)
				pAux->pPrev->pNext = ddTask;

			pAux->pPrev = ddTask;


			// if inserting at the front, move the head pointer
			if (pAux == list->pHead )
				list->pHead = ddTask;

			// set the priority to 1 more than pAux priority
			ddTask->xPriority = pAux->xPriority  + 1;
			vTaskPrioritySet(ddTask->xTask, ddTask->xPriority);

			break; // early return from here

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

				break;
			}

			pAux = pAux->pNext; // no early return, want to keep iterating
					// or, if at end of list, then we just inserted,
					// and we can safely fall out of the loop and return

		}
	}

	(list->uSize)++;

	SafePrintFromTask(DEBUG_LIST, "List Size after insertion: %d\n", list->uSize);
	return DD_Success;
}

// traverse the list front the front, modifying deadlines until the removal point is found
DD_Status_t	DD_TaskListRemoveByHandle(DD_TaskListHandle_t list, DD_TaskHandle_t ddTask)
{

	/* ----------- VERIFY ALL PRECONDITIONS ------------- */
	// check that the arguments are not null
	if (list == NULL || ddTask == NULL)
	{
		SafePrintFromTask(DEBUG_LIST, "Null Parameter\n");
		return DD_Argument_Null;
	}

	// if the list is empty, just return
	if (DD_TaskListIsEmpty(list))
	{
		SafePrintFromTask(DEBUG_LIST, "Task List is Empty\n");
		return DD_TaskList_TaskHandle_Not_Found;
	}



	// check that task is actually part of the list

	// auxiliary pointer for list traversal
	DD_TaskHandle_t pAux = list->pHead;

	// search for it in the list first to make sure we don't alter the list
	while (pAux != NULL)
	{
		if (pAux == ddTask)
			break;

		pAux = pAux->pNext;
	}

	// if we got to the end, then its not there
	if (pAux == NULL)
	{
		SafePrintFromTask(DEBUG_LIST, "Task Handle not in list");
		return DD_TaskList_TaskHandle_Not_Found;
	}

	/* ------------- REMOVE FROM LIST -------------------- */

	SafePrintFromTask(DEBUG_LIST, "Task list size before removal: %d\n", list->uSize);

	ddTask->xStatus = DD_TaskDeleted;

	// Since we got here, the item is in the list
	pAux = list->pHead;
	// reduce the priority of all the items in front of it
	while( pAux != ddTask )
	{
		pAux->xPriority = pAux->xPriority - 1; // reduce priority
		vTaskPrioritySet(ddTask->xTask, ddTask->xPriority);

		pAux = pAux->pNext;
	}

	// pAux now points to the task to remove
	// rearrange pointer relationships

	// only item in list
	if (DD_TaskListGetSize(list) == 1)
	{
		list->pHead = NULL;
		list->pTail = NULL;
		list->uSize = 0;

		pAux->pNext = NULL;
		pAux->pPrev = NULL;
	}
	// the head of a list with size > 1
	else if (pAux == list->pHead)
	{
		list->pHead = pAux->pNext;
		list->pHead->pPrev = NULL;

		pAux->pNext = NULL;
		pAux->pPrev = NULL;

		list->uSize--;
	}
	else if (pAux == list->pTail)
	{
		list->pTail = pAux->pPrev;
		list->pTail->pNext = NULL;

		pAux->pNext = NULL;
		pAux->pPrev = NULL;

		list->uSize--;
	}
	else
	{
		pAux->pPrev->pNext = pAux->pNext;
		pAux->pNext->pPrev = pAux->pPrev;

		pAux->pNext = NULL;
		pAux->pPrev = NULL;

		list->uSize--;
	}

	SafePrintFromTask(DEBUG_LIST, "Active Task List size after removal: %d\n", list->uSize);
	return DD_Success;
}

/*
// append list2 to list1
DD_Status_t	DD_TaskListConcatenate(DD_TaskListHandle_t list1, DD_TaskListHandle_t list2)
{
	if (list1 == NULL || list2 == NULL)
		return DD_Argument_Null;

	list1->uSize += list2->uSize;

	list1->pTail->pNext = list2->pHead;
	list2->pHead->pPrev = list1->pTail;
	list1->pTail = list2->pTail;

	return DD_Success;
}*/

DD_Status_t DD_TaskListInsertBack(DD_TaskListHandle_t list, DD_TaskHandle_t task)
{
	return DD_Success;
}

char* DD_TaskListDataReturn(DD_TaskListHandle_t list)
{

	u32 size = DD_TaskListGetSize(list);
	u32 dynamicSize = (size + 1)*(100);
	char* data = (char*)pvPortMalloc(dynamicSize*sizeof(char));
	data[0] = '\0'; // instead of zeroing the whole thing, spoof it by making the first char null

	DD_TaskHandle_t pAux = list->pHead;


	int hfDebug = 0;
	while(pAux != NULL)
	{
		char buffer[200];

		char* temp1;
		switch(pAux->xStatus)
		{
			case DD_TaskUninitialized:
				temp1 = "Uninitialized";
				break;
			case DD_TaskActive:
				temp1 = "Active";
				break;
			case DD_TaskOverdue:
				temp1 = "Overdue";
				break;
			case DD_TaskDeleted: // should NEVER SEE THIS
				temp1 = "Deleted";
				break;
		}
		sprintf(buffer,
				"Task: %s\tPriority: %d\tAbs Deadline: %u\tStatus: %s\n",
				pAux->sTaskName,
				(int)pAux->xPriority,
				(unsigned int)pAux->xAbsDeadline,
				temp1);
		strcat(data,buffer);

		hfDebug++;
		pAux = pAux->pNext;
	}


	return data;
}


// collect all overdue tasks from the active list and put at the end of the overdue list
DD_Status_t DD_TaskListRemoveOverdue(DD_TaskListHandle_t active, DD_TaskListHandle_t overdue, TickType_t currentTime)
{
	if (active == NULL || overdue == NULL)
	{
		SafePrintFromTask(DEBUG_LIST, "Null Parameter\n");
		return DD_Argument_Null;
	}

	SafePrintFromTask(DEBUG_LIST, "Active Task list size before removal: %d\n", active->uSize);

	DD_TaskHandle_t pIter = active->pHead;
	DD_TaskHandle_t pCurr;


	// iterate through the active list starting from the front
	// all overdue tasks will be at the front since the list is sorted by deadline
	// if the list is empty, pHead is null, and the following loop won't run
	// if the entire list is overdue, then the last item will be the tail and we need to take a special action
	while (pIter != NULL && currentTime >= pIter->xAbsDeadline)
	{
		// if the task has a resource, then this is still safe
		// since the idle task cleans up resources
		SafePrintFromTask(DEBUG_LIST, "Task %s is overdue\n", pIter->sTaskName);

		if (pIter->xTask != NULL)// equivalent to pIter->xStatus != DD_TaskOverdue
		{	// if its already null, then it was killed by the software callback
			vTaskSuspend(pIter->xTask);
			vTaskDelete(pIter->xTask);
		}
		pIter->xStatus = DD_TaskOverdue;

		pCurr = pIter; // keep a reference to the current one
		pIter = pIter->pNext; // move the next pointer to the next one

		// remove the item off the front and put it on the end of the overdue list
		// on every iteration, pAux is garanteed to point to the front of the list
		// it is always the front of the list

		// remove from list
		active->pHead = pCurr->pNext; // == pIter
		pCurr->pPrev = NULL;

		if (pCurr != active->pTail) // also: pIter != NULL, pCurr->pNext != NULL
			pCurr->pNext->pPrev = NULL;

		active->uSize--;

		// add to overdue
		pCurr->pPrev = overdue->pTail;
		overdue->pTail = pCurr;

		if (overdue->pHead == NULL) // adding first item
			overdue->pHead = pCurr;

		overdue->uSize++;
	}

	SafePrintFromTask(DEBUG_LIST, "Active Task List size after removal: %d\n", active->uSize);
	return DD_Success;
}

