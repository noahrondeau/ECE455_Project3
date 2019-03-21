/*
 * Message.h
 *
 *  Created on: Mar 21, 2019
 *      Author: ntron
 */

#ifndef DD_MESSAGE_H_
#define DD_MESSAGE_H_

#include "config.h"

typedef enum DD_MessageType_t
{
	DD_Message_TaskCreate,
	DD_Message_TaskDelete,
	DD_Message_GetActiveList,
	DD_Message_GetOverdueList,
} DD_MessageType_t;

typedef struct DD_Message_t
{
	DD_MessageType_t msg;
	TaskHandle_t sender;
	void* data;
} DD_Message_t;




#endif /* DD_MESSAGE_H_ */
