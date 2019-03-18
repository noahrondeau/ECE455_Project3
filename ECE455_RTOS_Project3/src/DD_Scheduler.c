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

/* ---------------- PUBLIC INTERFACE ------------------ */
TaskHandle_t	DD_TaskCreate(DD_Task_t tparams)
{
	//TODO
	return NULL;
}

DD_Status_t 	DD_TaskDelete(DD_Task_t xTask)
{
	//TODO
	return 0;
}

DD_Status_t		DD_ReturnActiveList(DD_TaskList_t* retActiveList)
{
	//TODO : return either a copy of the list or a pointer to it
	return 0;
}

DD_Status_t		DD_ReturnOverdueList(DD_TaskList_t* retOverdueList)
{
	//TODO : return either a copy of the list or a pointer to it
	return 0;
}


