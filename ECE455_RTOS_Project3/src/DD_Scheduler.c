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

	//Checks if queue created successfully
	if(DDChannel_Create == NULL)
	{
		printf("Error creating queue\n");
		//Code here for if not created successfully
	}

	//created the task passed in through params
	xTaskCreate(ddTask->xFunction,ddTask->sTaskName,ddTask->uStackSize,NULL,ddTask->xPriority,&(ddTask->xTask));
	printf("Task created\n");
	/*send a message to the scheduler containing the task
	 * Scheduler will check to see if the queue is empty, if not receive message, do its thing then empty queue
	 */
	xQueueSend(DDChannel_Create,&ddTask,10);
	printf("Message sent to scheduler\n");

	//If queue is not empty, message not received and wait before destroying channel
	while(uxQueueSpacesAvailable(DDChannel_Create) == 0);
	vQueueDelete(DDChannel_Create);

	//Task Creation success
	printf("DD_TaskCreate Success\n");

	return DD_Success;
}

DD_Status_t 	DD_TaskDelete(TaskHandle_t xTask)
{
	//Opens the Queue by creating it
	DDChannel_Delete = xQueueCreate(1,sizeof(TaskHandle_t));

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
	xQueueSend(DDChannel_Delete,xTask,10);
	printf("Message sent to scheduler for task deletion \n");

	//If queue is not empty, message not received and wait before destroying channel
	while(uxQueueSpacesAvailable(DDChannel_Delete) == 0);
	vQueueDelete(DDChannel_Delete);
	//Task Deletion success
	printf("DD_TaskDelete Message Success \n");

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


