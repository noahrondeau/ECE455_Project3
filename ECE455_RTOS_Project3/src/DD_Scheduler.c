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
static QueueHandle_t xMonitorQueue;

static DD_Status_t DD_SchedulerInit(void);
static DD_Status_t DD_MonitorInit(void);
DD_Status_t DD_SchedulerStart(void);

void DD_SchedulerTaskFunction( void* pvParameters );
void vMonitorTask(void* pvParameters);

void MockTaskListFunction(DD_TaskListHandle_t ActiveList,DD_TaskListHandle_t OverdueList);


/* ---------------- PRIVATE FUNCTIONS ------------------ */

static DD_Status_t DD_SchedulerInit()
{
	DD_TaskListInit(&xActiveTaskList);
	DD_TaskListInit(&xOverdueTaskList);


	xMessageQueue = xQueueCreate(SCHEDULER_MAX_USER_TASKS_NUM, sizeof(DD_Message_t));
	vQueueAddToRegistry(xMessageQueue,"Message Queue");
	xTaskCreate(DD_SchedulerTaskFunction, "DD_Scheduler", configMINIMAL_STACK_SIZE, NULL,DD_TASK_PRIOTITY_MONITOR, NULL);

	if ( xMessageQueue == NULL)
		return DD_Queue_Open_Fail;

	return DD_Success;

}

static DD_Status_t DD_MonitorInit()
{
	// TODO: create monitor task and configure anything necessary
	xMonitorQueue = xQueueCreate(1,sizeof( DD_Message_t));
	if(xMonitorQueue == NULL) return DD_Queue_Open_Fail;
	vQueueAddToRegistry(xMonitorQueue,"Monitor Queue");
	xTaskCreate(vMonitorTask,"DD_Monitor",configMINIMAL_STACK_SIZE, NULL, DD_TASK_PRIORITY_SCHEDULER, NULL);

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
				// Notify the message sender its message is being processed
				DebugSafePrint("Received create message for task %s\n", ((DD_TaskHandle_t)(xReceivedMessage.data))->sTaskName);

				xTaskNotifyGive(xReceivedMessage.sender);
			}
			break;

			case DD_Message_TaskDelete:
			{
				// TODO:	run scheduling algorithms
				//			remove item from list and change priorities
				//			move overdue stuff
				DebugSafePrint("Received delete message for task %s\n", ((DD_TaskHandle_t)(xReceivedMessage.data))->sTaskName);

				xTaskNotifyGive(xReceivedMessage.sender);
			}
			break;

			case DD_Message_GetActiveList:
			{
				// TODO: everything
				DebugSafePrint("Received Active List request\n");

				xReceivedMessage.data = (void*)DD_TaskListDataReturn(&xActiveTaskList);

				 if( xMonitorQueue != 0 )
				    {
				        if( xQueueSend( xMonitorQueue,
				                       ( void * ) &xReceivedMessage,
				                       ( TickType_t ) 10 ) != pdPASS )
				        {
				        	DebugSafePrint("Message sent to MonitorTask failed");
				        }
				    }

			}
			break;

			case DD_Message_GetOverdueList:
			{
				// TODO: everything
				DebugSafePrint("Received Overdue List request\n");
				xReceivedMessage.data = (void*)DD_TaskListDataReturn(&xOverdueTaskList);

				 if( xMonitorQueue != 0 )
				    {
				        if( xQueueSend( xMonitorQueue,
				                       ( void * ) &xReceivedMessage,
				                       ( TickType_t ) 10 ) != pdPASS )
				        {
				        	DebugSafePrint("Message sent to MonitorTask failed");
				        }
				    }
			}
			break;
			}
		}
	}
}

void vMonitorTask(void* pvParameters)
{
	unsigned int taskCount = 0;
	taskCount = uxTaskGetNumberOfTasks();
	DebugSafePrint("Number of tasks at FIRST mock run is: %d\n", taskCount);

	while(1)
	{
		taskCount = uxTaskGetNumberOfTasks();
		DebugSafePrint("Number of tasks at mock run is: %d\n", taskCount);

		DD_ReturnActiveList();
		DD_ReturnOverdueList();

		vTaskDelay(5000);
	}
}

/* ---------------- Scheduler Test Functions ------------------ */

void MockTaskListFunction(DD_TaskListHandle_t ActiveList,DD_TaskListHandle_t OverdueList)
{
	//node creation
	DD_TaskHandle_t active1 = DD_TaskAlloc();
	DD_TaskHandle_t active2 = DD_TaskAlloc();
	DD_TaskHandle_t active3 = DD_TaskAlloc();
	DD_TaskHandle_t active4 = DD_TaskAlloc();
	DD_TaskHandle_t active5 = DD_TaskAlloc();
	//Task Naming
	active1->sTaskName="Active1";
	active2->sTaskName="Active2";
	active3->sTaskName="Active3";
	active4->sTaskName="Active4";
	active5->sTaskName="Active5";
	//List assignment
	ActiveList->pHead = active1;
	ActiveList->pTail = active5;
	ActiveList->uSize = 5;
	//Node next linkage
	active1->pNext=active2;
	active2->pNext=active3;
	active3->pNext=active4;
	active4->pNext=active5;
	//Node prev linkage
	active5->pPrev=active4;
	active4->pPrev=active3;
	active3->pPrev=active2;
	active2->pPrev=active1;

	//node creation
	DD_TaskHandle_t od1 = DD_TaskAlloc();
	DD_TaskHandle_t od2 = DD_TaskAlloc();
	DD_TaskHandle_t od3 = DD_TaskAlloc();
	DD_TaskHandle_t od4 = DD_TaskAlloc();
	DD_TaskHandle_t od5 = DD_TaskAlloc();
	//Task Naming
	od1->sTaskName="od1";
	od2->sTaskName="od2";
	od3->sTaskName="od3";
	od4->sTaskName="od4";
	od5->sTaskName="od5";
	//List assignment
	OverdueList->pHead = od1;
	OverdueList->pTail = od5;
	OverdueList->uSize = 5;
	//Node next linkage
	od1->pNext=od2;
	od2->pNext=od3;
	od3->pNext=od4;
	od4->pNext=od5;
	//Node prev linkage
	od5->pPrev=od4;
	od4->pPrev=od3;
	od3->pPrev=od2;
	od2->pPrev=od1;
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


	//TODO: List testing
	MockTaskListFunction(&xActiveTaskList,&xOverdueTaskList);

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

DD_Status_t		DD_Queue_Init(void){

	xMonitorQueue = xQueueCreate(1,sizeof( DD_Message_t));
	xMessageQueue = xQueueCreate(1,sizeof( DD_Message_t));

	if(xMonitorQueue == NULL || xMessageQueue == NULL) return DD_Queue_Open_Fail;

	return DD_Success;
}

DD_Status_t		DD_ReturnActiveList(void)
{
	//TODO : return either a copy of the list or a pointer to it
	DD_Message_t xActiveRequest = {
		DD_Message_GetActiveList,
		xTaskGetCurrentTaskHandle(),
		NULL,
	};

	 if( xMessageQueue != NULL )
	    {
	        if( xQueueSend( xMessageQueue,
	                       ( void * ) &xActiveRequest,
	                       ( TickType_t ) 10 ) != pdPASS )
	        {
	            return DD_Message_Send_Fail;
	        }
	    }

	 if( xMonitorQueue != NULL )
	    {
	        if( xQueueReceive( xMonitorQueue, (void*)&xActiveRequest, ( TickType_t ) portMAX_DELAY ) )
	        {
	        	SafePrint(true,"Active Task List:\n%s\n",(char*)(xActiveRequest.data));
	        	vPortFree(xActiveRequest.data);
	        	xActiveRequest.data = NULL;
	        }
	    }


	return DD_Success;
}

DD_Status_t		DD_ReturnOverdueList(void)
{
	DD_Message_t xOverdueRequest = {
		DD_Message_GetOverdueList,
		xTaskGetCurrentTaskHandle(),
		NULL,
	};

	 if( xMessageQueue != NULL )
	    {
	        if( xQueueSend( xMessageQueue,
	                       ( void * ) &xOverdueRequest,
	                       ( TickType_t ) 10 ) != pdPASS )
	        {
	            return DD_Message_Send_Fail;
	        }
	    }

	 if( xMonitorQueue != NULL )
	    {
	        if( xQueueReceive( xMonitorQueue, (void*)&xOverdueRequest, ( TickType_t ) portMAX_DELAY ) )
	        {
	        	SafePrint(true,"Overdue Task List:\n%s\n",(char*)(xOverdueRequest.data));
	        	vPortFree(xOverdueRequest.data);
	        	xOverdueRequest.data = NULL;
	        }
	    }


	return DD_Success;
}



