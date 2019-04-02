/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

/* Application Includes */
#include "config.h"
#include "DD_Scheduler.h"
#include "SafePrint.h"
#include "DD_Aux.h"


static void prvSetupHardware( void );


void vTestPeriodic1(void* pvParameters);
void vTestPeriodic2(void* pvParameters);
void vTestPeriodic3(void* pvParameters);
void vTestAperiodic1(void* pvParameters);
void vGenPeriodic1( void* pvParameters);
void vGenPeriodic2( void* pvParameters);
void vGenPeriodic3( void* pvParameters);
void vGenAperiodic1( void* pvParameters);
void vMonitorTask(void* pvParameters);

TaskHandle_t xGenPeriodic1Handle;
TaskHandle_t xGenPeriodic2Handle;
TaskHandle_t xGenPeriodic3Handle;
TaskHandle_t xGenAperiodic1Handle;

// This task uses its exec time fully
#define P1_PERIOD	(10000)
#define P1_EXEC	(4000)
#define P1_LED_RATE (250)

// This task uses its exec time fully
#define P2_PERIOD (20000)
#define P2_EXEC	(7500)
#define P2_LED_RATE (500)

// This task delays for twice its period and will be overdue
#define P3_PERIOD (10000)
//#define P2_EXEC	(0)
//#define P2_LED_RATE (500)

#define A1_DEAD	(10000)
#define A1_EXEC	(5000)
#define A1_LED_RATE	(50)

/*-----------------------------------------------------------*/

int main(void)
{
	prvSetupHardware();
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 6);
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	SafePrintInit();

	/* Start the tasks and timer running. */
	xTaskCreate(vGenPeriodic1, "PG1", configMINIMAL_STACK_SIZE, NULL, DD_TASK_GEN_PRIORITY_PERIODIC, &xGenPeriodic1Handle);
	xTaskCreate(vGenPeriodic2, "PG2", configMINIMAL_STACK_SIZE, NULL, DD_TASK_GEN_PRIORITY_PERIODIC, &xGenPeriodic2Handle);
	xTaskCreate(vGenPeriodic3, "PG3", configMINIMAL_STACK_SIZE, NULL, DD_TASK_GEN_PRIORITY_PERIODIC, &xGenPeriodic3Handle);
	xTaskCreate(vGenAperiodic1, "AG1", configMINIMAL_STACK_SIZE, NULL, DD_TASK_GEN_PRIORITY_APERIODIC, &xGenAperiodic1Handle);
	DD_SchedulerStart(); // starts the DD_Scheduler and the FreeRTOS scheduler


	return 0;
}


/*-----------------------------------------------------------*/

// push button
void EXTI0_IRQHandler(void)
{
	/* Make sure that interrupt flag is set */
	if (EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
		STM_EVAL_LEDToggle(LED4);
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(xGenAperiodic1Handle, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}



/*-----------------------------------------------------------*/
//TEST TASKS
void vTestPeriodic1(void* pvParameters)
{
	// get self item from params
	DD_TaskHandle_t ddSelf = (DD_TaskHandle_t)pvParameters;

	TickType_t xTickCurr;
	TickType_t xTickPrev;
	while(1)
	{
		xTickCurr = xTaskGetTickCount();
		if ( xTickCurr <= ddSelf->xCreationTime + P1_EXEC)
		{
			if (xTickCurr != xTickPrev)
			{
				if (xTickCurr % P1_LED_RATE == 0)
				{
					STM_EVAL_LEDToggle(LED5);
				}
			}
		}
		else
		{
			//Test the removal by having task 1 never delete itself
			STM_EVAL_LEDOff(LED5);
			DD_TaskDelete(ddSelf);
		}

		xTickPrev = xTickCurr;
	}
}

void vTestPeriodic2(void* pvParameters)
{
	// get self item from params
	DD_TaskHandle_t ddSelf = (DD_TaskHandle_t)pvParameters;

	TickType_t xTickCurr;
	TickType_t xTickPrev;
	while(1)
	{
		xTickCurr = xTaskGetTickCount();
		if ( xTickCurr <= ddSelf->xCreationTime + P2_EXEC )
		{
			if (xTickCurr != xTickPrev)
			{
				if (xTickCurr % P2_LED_RATE == 0)
				{
					STM_EVAL_LEDToggle(LED6);
				}
			}
		}
		else
		{
			STM_EVAL_LEDOff(LED6);
			DD_TaskDelete(ddSelf);
		}

		xTickPrev = xTickCurr;
	}
}

void vTestPeriodic3(void* pvParameters)
{
	// get self item from params
	DD_TaskHandle_t ddSelf = (DD_TaskHandle_t)pvParameters;

	TickType_t xTickCurr;
	TickType_t xTickPrev;
	while(1)
	{
		STM_EVAL_LEDToggle(LED3);
		vTaskDelay(2*P3_PERIOD);
		DD_TaskDelete(ddSelf);
	}
}

void vTestAperiodic1(void* pvParameters)
{
	// get self item from params
	DD_TaskHandle_t ddSelf = (DD_TaskHandle_t)pvParameters;

	TickType_t xTickCurr;
	TickType_t xTickPrev;
	while(1)
	{
		xTickCurr = xTaskGetTickCount();
		if ( xTickCurr <= ddSelf->xCreationTime + A1_EXEC)
		{
			if (xTickCurr != xTickPrev)
			{
				if (xTickCurr % A1_LED_RATE == 0)
				{
					STM_EVAL_LEDToggle(LED4);
				}
			}
		}
		else
		{
			STM_EVAL_LEDOff(LED4);
			DD_TaskDelete(ddSelf);
		}

		xTickPrev = xTickCurr;
	}
}

// period: 5000
// deadline: 5000
// exec: 2500
void vGenPeriodic1( void* pvParameters)
{
	while(1)
	{
		DD_TaskHandle_t ddTestTask = DD_TaskAlloc();
		ddTestTask->sTaskName = "P1";
		ddTestTask->xFunction = vTestPeriodic1;
		ddTestTask->xRelDeadline = P1_PERIOD;
		ddTestTask->xTaskType = DD_TaskPeriodic;

		DD_TaskCreate(ddTestTask);
		vTaskDelay(P1_PERIOD);// wait another 25 s before running again
	}
}

// period: 10000
// deadline: 10000
// exec: 5000
void vGenPeriodic2( void* pvParameters)
{
	while(1)
	{
		DD_TaskHandle_t ddTestTask = DD_TaskAlloc();
		ddTestTask->sTaskName = "P2";
		ddTestTask->xFunction = vTestPeriodic2;
		ddTestTask->xRelDeadline = P2_PERIOD;
		ddTestTask->xTaskType = DD_TaskPeriodic;

		DD_TaskCreate(ddTestTask);
		vTaskDelay(P2_PERIOD);// wait another 25 s before running again
	}
}

void vGenPeriodic3( void* pvParameters)
{
	while(1)
	{
		DD_TaskHandle_t ddTestTask = DD_TaskAlloc();
		ddTestTask->sTaskName = "P3";
		ddTestTask->xFunction = vTestPeriodic3;
		ddTestTask->xRelDeadline = P3_PERIOD;
		ddTestTask->xTaskType = DD_TaskPeriodic;

		DD_TaskCreate(ddTestTask);
		vTaskDelay(P3_PERIOD);// wait another 25 s before running again
	}
}

// deadline: 2500
// exec: 2000
void vGenAperiodic1( void* pvParameters) // Deferred ISR handler
{
	while(1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		DD_TaskHandle_t ddTestTask = DD_TaskAlloc();
		ddTestTask->sTaskName = "A1";
		ddTestTask->xFunction = vTestAperiodic1;
		ddTestTask->xRelDeadline = A1_DEAD;
		ddTestTask->xTaskType = DD_TaskSporadic;

		DD_TaskCreate(ddTestTask);
	}
}



//PROJECT TASKS



/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

