/*
 * DD_Scheduler.c
 *
 *  Created on: Mar 14, 2019
 *      Author: ntron
 */


#include "DD_Scheduler.h"
#include "DD_Message.h"

/* ---------------- PRIVATE DEFINTIONS ----------------- */

static DD_TaskList_t xActiveTaskList;
static DD_TaskList_t xOverdueTaskList;
static QueueHandle_t xMessageQueue;

static DD_Status_t DD_SchedulerInit(void);
static DD_Status_t DD_MonitorInit(void);
DD_Status_t DD_SchedulerStart(void);

void DD_SchedulerTaskFunction( void* pvParameters );


/* ---------------- PRIVATE FUNCTIONS ------------------ */

static DD_Status_t DD_SchedulerInit()
{
	DD_TaskListInit(&xActiveTaskList);
	DD_TaskListInit(&xOverdueTaskList);

	xMessageQueue = xQueueCreate(SCHEDULER_MAX_USER_TASKS_NUM, sizeof(DD_Message_t));
	vQueueAddToRegistry(xMessageQueue,"Message Queue");
	//xTaskCreate(SCHEDULER TASK);

	if ( xMessageQueue == NULL)
		return DD_Queue_Open_Fail;

	return DD_Success;

}

static DD_Status_t DD_MonitorInit()
{
	// TODO: create monitor task and configure anything necessary
	// xTaskCreate(MONITOR TASK);
	return DD_Success;
}


void DD_SchedulerTaskFunction( void* pvParameters )
{
	DD_Message_t xReceivedMessage;

	while (true)
	{
		// wait forever for the next message to arrive
		if (xQueueReceive(xMessageQueue, (void*)&xReceivedMessage, portMAX_DELAY) == pdTRUE)
		{
			switch (xReceivedMessage.msg)
			{
			case DD_Message_TaskCreate:
			{
				// TODO:	run scheduling algorithms
				//			insert item and change priorities
				//			move overdue stuff
				// Notify the
				xTaskNotifyGive(xReceivedMessage.sender);
			}
			break;

			case DD_Message_TaskDelete:
			{
				// TODO:	run scheduling algorithms
				//			remove item from list and change priorities
				//			move overdue stuff
				xTaskNotifyGive(xReceivedMessage.sender);
			}
			break;

			case DD_Message_GetActiveList:
			{
				// TODO: everything
			}
			break;

			case DD_Message_GetOverdueList:
			{
				// TODO: everything
			}
			break;
			}
		}
	}
}


/* ---------------- PUBLIC INTERFACE ------------------ */


DD_Status_t DD_SchedulerStart()
{
	DD_Status_t status = DD_None;

	status = DD_SchedulerInit();
	if (status != DD_Success)
		return status;

	status = DD_MonitorInit();
	if (status != DD_Success)
		return status;

	vTaskStartScheduler();

	return DD_Success;
}


/*
1. Opens a queue
2. Creates the task specified and assigns it the minimum priority possible
3. Composes a create_task_message and sends it to the DD-scheduler
4. Waits for a reply at the queue it created above
5. Once the reply is received, it obtains it
6. Destroys the queue
7. Returns to the invoking task
*/

/* Intended operation:
 * DD_TaskCreate will create a queue of the size 1 of length of the task struct
 * Creates the task based on the contents of the struct
 * Sends a message to the the Scheduler using the queue containing the struct
 * Waits for Scheduler to empty the Queue, once emptied the queue will be deleted
 *
 * DD_TaskDelete will create a queue of the size 1 of length of the task struct
 * Will delete the task using the taskhandle in the passed in struct
 * Sends a message to the the Scheduler using the queue containing the struct
 * Waits for Scheduler to empty the Queue, once emptied the queue will be deleted
 */

DD_Status_t	DD_TaskCreate(DD_TaskHandle_t ddTask)
{
	// Get the current tick to set the absolute deadline
	ddTask->xCreationTime = xTaskGetTickCount();
	ddTask->xAbsDeadline = ddTask->xCreationTime + ddTask->xRelDeadline;

	//create the task
	xTaskCreate(ddTask->xFunction,
				ddTask->sTaskName,
				ddTask->uStackSize,
				(void*)ddTask, // passing in the DD_TaskHandle_t as pvParameters so task is aware of its own params
				ddTask->xPriority,
				&(ddTask->xTask));

	printf("Task created\n");

	// Send message to the scheduler process, block forever if the queue is full

	DD_Message_t message = {
			.msg = DD_Message_TaskCreate,
			.sender = xTaskGetCurrentTaskHandle(),
			.data = (void*)ddTask,
	};

	xQueueSend(xMessageQueue, (void*)&message, portMAX_DELAY);

	printf("Message sent to scheduler\n");

	// Wait for task notification from scheduler
	ulTaskNotifyTake( pdTRUE, portMAX_DELAY);

	/* NOTE: there really should really be some checks to make sure the operation was successful
	 * we can do that later
	 */

	//Task Creation success
	printf("DD_TaskCreate Success\n");

	return DD_Success;
}

DD_Status_t 	DD_TaskDelete(DD_TaskHandle_t ddTask)
{

	/*Send a message to the scheduler containing the task struct
	 *Scheduler will check to see if the queue is empty, if not receive message, do its thing then empty queue
	 */

	DD_Message_t message = {
		.msg = DD_Message_TaskDelete,
		.sender = xTaskGetCurrentTaskHandle(),
		.data = (void*)ddTask,
	};

	xQueueSend(xMessageQueue, (void*)&message, portMAX_DELAY);
	printf("Message sent to scheduler for task deletion \n");

	// wait for notification that the operation is finished
	ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

	// received notification, it is safe to deallocate the task handle
	// note that deallocating the DD_TaskHandle_t does not deallocate the TaskHandle_t location
	// allocated by xTaskCreate

	if ( DD_TaskDealloc(ddTask) != DD_Success )
	{
		printf("Task pointers to list not NULL!\n");
		// do it again
		// this is NOT the optimum solution, its just t prevent overflow
		// while we debug any reason that might lead to this condition
		ddTask->pNext = NULL;
		ddTask->pPrev = NULL;
		DD_TaskDealloc(ddTask);
	}

	// task deletes itself
	vTaskDelete(NULL);
	return DD_Success;
}

DD_Status_t		DD_ReturnActiveList(DD_TaskListHandle_t retActiveList)
{
	//TODO : return either a copy of the list or a pointer to it
	return DD_None;
}

DD_Status_t		DD_ReturnOverdueList(DD_TaskListHandle_t retOverdueList)
{
	//TODO : return either a copy of the list or a pointer to it
	return DD_None;
}



