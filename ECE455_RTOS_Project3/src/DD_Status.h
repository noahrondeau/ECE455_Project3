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
	DD_Error_Queue_Open_Fail,
	DD_Error_Queue_Close_Fail,
} DD_Status_t;

#endif /* DD_STATUS_H_ */
