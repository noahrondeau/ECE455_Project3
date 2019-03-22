/*
 * DD_Status.h
 *
 *  Created on: Mar 18, 2019
 *      Author: ntron
 */

#ifndef DD_STATUS_H_
#define DD_STATUS_H_

typedef enum DD_Status_t
{
	DD_None,
	DD_Success,
	DD_Failure,
	DD_Queue_Open_Fail,
	DD_Queue_Close_Fail,
	DD_Message_Send_Fail,
	DD_Task_Alloc_Fail_xTask_Not_Created,
	DD_Task_Dealloc_Fail_xTask_Not_Null,
	DD_Task_Dealloc_Fail_pNext_Not_Null,
	DD_Task_Dealloc_Fail_pPrev_Not_Null,
} DD_Status_t;

#endif /* DD_STATUS_H_ */
