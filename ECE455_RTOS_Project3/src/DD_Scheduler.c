/*
 * DD_Scheduler.c
 *
 *  Created on: Mar 14, 2019
 *      Author: ntron
 */


#include "DD_Scheduler.h"
#include "DD_Message.h"
#include "SafePrint.h"

/* ---------------- PRIVATE DEFINTIONS ----------------- */

static DD_TaskList_t xActiveTaskList;
static DD_TaskList_t xOverdueTaskList;
static QueueHandle_t xMessageQueue;

static DD_Status_t DD_SchedulerInit(void);
static DD_Status_t DD_MonitorInit(void);
DD_Status_t DD_SchedulerStart(void);

void DD_SchedulerTaskFunction( void* pvParameters );
static void DD_SporadicTaskTimerCallback(TimerHandle_t xTimer);


/* ---------------- PRIVATE FUNCTIONS ------------------ */

static DD_Status_t DD_SchedulerInit()
{
	DD_TaskListInit(&xActiveTaskList);
	DD_TaskListInit(&xOverdueTaskList);

	xMessageQueue = xQueueCreate(SCHEDULER_MAX_USER_TASKS_NUM, sizeof(DD_Message_t));
	vQueueAddToRegistry(xMessageQueue,"Message Queue");
	xTaskCreate(DD_SchedulerTaskFunction, "DD_Scheduler", configMINIMAL_STACK_SIZE, NULL, DD_TASK_PRIORITY_SCHEDULER, NULL);

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
	TickType_t xTick = xTaskGetTickCount();
	DD_TaskHandle_t xReceivedTask = NULL;

	while (true)
	{
		// wait forever for the next message to arrive
		if (xQueueReceive(xMessageQueue, (void*)&xReceivedMessage, portMAX_DELAY) == pdTRUE)
		{
			xTick = xTaskGetTickCount();
			xReceivedTask = (DD_TaskHandle_t)(xReceivedMessage.data); // only valid in certain cases of the switch

			switch (xReceivedMessage.msg)
			{
			case DD_Message_TaskCreate:
			{
				if ( (xReceivedTask == NULL) || (xReceivedTask->xTask == NULL) )
				{
					// ERROR!
					DebugSafePrint("Received create message but task handle was null");
				}
				else
				{
					DebugSafePrint("Received create message for task %s\n", xReceivedTask->sTaskName);
					DD_TaskListRemoveOverdue(&xActiveTaskList, &xOverdueTaskList, xTick);
					DD_TaskListInsertByDeadline(&xActiveTaskList, xReceivedTask);

					// if the task is marked sporadic, start a timer that will go off at the deadline
					// this timer will delete the FreeRTOS task (not the DD_Task_t)
					if (xReceivedTask->xTaskType == DD_TaskSporadic)
					{
						xReceivedTask->xTimer = xTimerCreate(
								xReceivedTask->sTaskName,
								xReceivedTask->xAbsDeadline - xTick,
								pdFALSE,
								(void*)xReceivedTask,
								DD_SporadicTaskTimerCallback);

						xTimerStart(xReceivedTask->xTimer, 0); // no block
					}

					vTaskResume(xReceivedTask->xTask);
					xTaskNotifyGive(xReceivedMessage.sender);
				}
			}
			break;

			case DD_Message_TaskDelete:
			{
				if ( (xReceivedTask == NULL) || (xReceivedTask->xTask == NULL) )
				{
					// ERROR!
					DebugSafePrint("Received create message but task handle was null");
				}
				else
				{
					DebugSafePrint("Received delete message for task %s\n", xReceivedTask->sTaskName);

					// if the task is sporadic, and
					// if the timer callback hasn't already gone off, stop the timer
					if (	(xReceivedTask->xTaskType == DD_TaskSporadic)
							&& (xReceivedTask->xStatus != DD_TaskOverdue) )
					{
						xTimerStop(xReceivedTask->xTimer, 0); // no block
						//DebugSafePrint("Task was not overdue yet");
					}

					DD_TaskListRemoveByHandle(&xActiveTaskList, xReceivedTask);
					DD_TaskListRemoveOverdue(&xActiveTaskList, &xOverdueTaskList, xTick); // remove overdue after in case the one we are deleting is overdue
					xTaskNotifyGive(xReceivedMessage.sender);
				}
			}
			break;

			/*// This job is currently taking care of as garbage collection by the monitor task and any time the scheduler is invoked
			case DD_Message_TaskDelete_SporadicOverdue:
			{
				DebugSafePrint("Received delete message for overdue sporadic task %s\n", xReceivedTask->sTaskName);
				DD_TaskListRemoveOverdue(&xActiveTaskList, &xOverdueTaskList, xTick); // remove overdue after in case the one we are deleting is overdue
			}
			break;
			*/

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

static void DD_SporadicTaskTimerCallback(TimerHandle_t xTimer)
{
	// ID field is used to store pointer to the DD_TaskHandle_t of the associated task
	// this task handle also contains the xTimer pointer, i.e. ddTaskToDelete->xTimer == xTimer
	DD_TaskHandle_t ddTaskToDelete = (DD_TaskHandle_t)pvTimerGetTimerID( xTimer );

	xTimerDelete(ddTaskToDelete->xTimer, 0);
	ddTaskToDelete->xTimer = NULL;

	// immediately suspend and delete the overdue task
	vTaskSuspend(ddTaskToDelete->xTask);
	vTaskDelete(ddTaskToDelete->xTask);

	ddTaskToDelete->xStatus = DD_TaskOverdue; // mark it overdue immediately so that we can check this later
	DebugSafePrint("Aperiodic task %s overdue and killed by scheduler\n", ddTaskToDelete->sTaskName);

	// for now, we just let the regular scheduler functionality deal with
	// garbage-collecting this overdue task into the overdue list
	// since the "remove overdue" routine is run on every single invocation of the DD_Scheduler
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

	if (ddTask->xTask == NULL)
	{
		DebugSafePrint("Failed to create task %s\n", ddTask->sTaskName);
		return DD_Failure;
	}

	// suspend the task, it will be made ready by the scheduler again
	vTaskSuspend(ddTask->xTask);

	// Send message to the scheduler process, block forever if the queue is full

	DD_Message_t message = {
			.msg = DD_Message_TaskCreate,
			.sender = xTaskGetCurrentTaskHandle(),
			.data = (void*)ddTask,
	};

	DebugSafePrint("Sending TaskCreate message to DD_Scheduler\n");
	xQueueSend(xMessageQueue, (void*)&message, portMAX_DELAY);

	// Wait for task notification from scheduler
	ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
	DebugSafePrint("Received TaskCreate ACK from DD_Scheduler\n");

	/* NOTE: there really should really be some checks to make sure the operation was successful
	 * we can do that later
	 */

	//Task Creation success
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

	DebugSafePrint("Sending TaskDelete message to scheduler\n");
	xQueueSend(xMessageQueue, (void*)&message, portMAX_DELAY);

	// wait for notification that the operation is finished
	ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
	DebugSafePrint("Received TaskDelete ACK from DD_Scheduler\n");

	// received notification, it is safe to deallocate the task handle
	// note that deallocating the DD_TaskHandle_t does not deallocate the TaskHandle_t location
	// allocated by xTaskCreate

	if ( DD_TaskDealloc(ddTask) != DD_Success )
	{
		DebugSafePrint("Task pointers to list not NULL!\n");
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



