/*
 * DD_Scheduler.c
 *
 *  Created on: Mar 14, 2019
 *      Author: ntron
 */


#include "DD_Scheduler.h"

/* ---------------- PRIVATE DEFINTIONS ----------------- */

static DD_TaskList_t xActiveTaskList;
static DD_TaskList_t xOverdueTaskList;

QueueHandle_t DDChannel_Create = NULL; //not sure if this is the appropriate place to put this
QueueHandle_t DDChannel_Delete = NULL;

static QueueHandle_t xMessageQueue;
static QueueHandle_t xMonitorQueue;


/* ---------------- PUBLIC INTERFACE ------------------ */

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
	//Opens the Queue by creating it
	DDChannel_Create = xQueueCreate(1,sizeof( DD_Task_t));
	vQueueAddToRegistry(DDChannel_Create,"Create Queue");

	//Checks if queue created successfully
	if(DDChannel_Create == NULL)
	{
		printf("Error creating queue\n");
		//Code here for if not created successfully
	}

	//created the task passed in through params
	xTaskCreate(ddTask->xFunction,ddTask->sTaskName,ddTask->uStackSize,(void*)ddTask,ddTask->xPriority,&(ddTask->xTask));
	printf("Task created\n");
	/*send a message to the scheduler containing the task
	 * Scheduler will check to see if the queue is empty, if not receive message, do its thing then empty queue
	 */
	xQueueSend(DDChannel_Create,&ddTask,10);
	printf("Message sent to scheduler\n");

	//If queue is not empty, message not received and wait before destroying channel
	while(uxQueueSpacesAvailable(DDChannel_Create) == 0);
	vQueueDelete(DDChannel_Create);

	DDChannel_Create = NULL;

	//Task Creation success
	printf("DD_TaskCreate Success\n");

	return DD_Success;
}

DD_Status_t 	DD_TaskDelete(DD_TaskHandle_t ddTask)
{
	//Opens the Queue by creating it
	DDChannel_Delete = xQueueCreate(1,sizeof(TaskHandle_t));
	vQueueAddToRegistry(DDChannel_Delete,"Delete Queue");
	//Checks if queue created successfully
	if(DDChannel_Delete == NULL)
	{
		printf("Error creating queue\n");
		//Code here for if not created successfully
	}

	//Deletes the task according to the task handle in the passed in struct
	//vTaskDelete(NULL);
	//printf("Task Deleted \n");

	/*Send a message to the scheduler containing the task struct
	 *Scheduler will check to see if the queue is empty, if not receive message, do its thing then empty queue
	 */
	xQueueSend(DDChannel_Delete,ddTask->xTask,10);
	printf("Message sent to scheduler for task deletion \n");

	//If queue is not empty, message not received and wait before destroying channel
	while(uxQueueSpacesAvailable(DDChannel_Delete) == 0);
	vQueueDelete(DDChannel_Delete);
	DDChannel_Delete = NULL;

	//Task Deletion success
	printf("DD_TaskDelete Message Success \n");
	printf("Task is being Deleted \n");
	DD_TaskDealloc(ddTask);
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

	 if( xMessageQueue != 0 )
	    {
	        if( xQueueSend( xMessageQueue,
	                       ( void * ) &xActiveRequest,
	                       ( TickType_t ) 10 ) != pdPASS )
	        {
	            return DD_Message_Send_Fail;
	        }
	    }

	 if( xMonitorQueue != 0 )
	    {
	        if( xQueueReceive( xMonitorQueue, &xActiveRequest, ( TickType_t ) 10 ) )
	        {
	        	char temp[sizeof((char*)xActiveRequest.data)];
	        	sprintf(temp,"Active List:\n %s ",(char*)xActiveRequest.data);
	        	vPortFree((char*)xActiveRequest.data);
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

	 if( xMessageQueue != 0 )
	    {
	        if( xQueueSend( xMessageQueue,
	                       ( void * ) &xOverdueRequest,
	                       ( TickType_t ) 10 ) != pdPASS )
	        {
	            return DD_Message_Send_Fail;
	        }
	    }

	 if( xMonitorQueue != 0 )
	    {
	        if( xQueueReceive( xMonitorQueue, &xOverdueRequest, ( TickType_t ) 10 ) )
	        {
	        	char temp[sizeof((char*)xOverdueRequest.data)];
	        	sprintf(temp,"Active List:\n %s ",(char*)xOverdueRequest.data);
	        	vPortFree((char*)xOverdueRequest.data);
	        }
	    }


	return DD_Success;
}


