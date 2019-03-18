/*
 * DD_TaskPriority.h
 *
 *  Created on: Mar 17, 2019
 *      Author: ntron
 */

#ifndef DD_TASKPRIORITY_H_
#define DD_TASKPRIORITY_H_

#include "config.h"

typedef u8 DD_TaskPriority_t;

/* Priorities on range 0 - 255 broken down according to:
 *
 * 0				--		IDLE // could actually potentially be at 1
 * 1				--		Not yet prioritized task by the scheduler
 * 2				--		Monitor Task
 * 3 - X			--		Reserved range for generator tasks // this may actually have to be high priority
 * (X+1) - 254		--		Reserved range for user tasks
 * 255				--		DD_Scheduler task priority
 */

// TODO figure out actual values
#define DD_TASK_PRIORITY_IDLE			(0) // Tentatively could be set to 1!
#define DD_TASK_PRIORITY_UNRELEASED		(1) // tentatively could be set to 0!
#define	DD_TASK_PRIOTITY_MONITOR		(2)
#define DD_TASK_GEN_RESERVED_MIN		(3)
#define DD_TASK_GEN_RESERVED_MAX		(20)
#define	DD_TASK_USER_RESERVED_MIN		(21)
#define DD_TASK_USER_RESERVED_MAX		(254)
#define DD_TASK_PRIORITY_SCHEDULER		(255)


#endif /* DD_TASKPRIORITY_H_ */
