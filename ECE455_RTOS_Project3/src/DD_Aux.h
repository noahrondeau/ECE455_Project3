/*
 * DD_Aux.h
 *
 *  Created on: Mar 18, 2019
 *      Author: Matthew
 */

#ifndef DD_AUX_H_
#define DD_AUX_H_

#include "config.h"
#include "DD_Scheduler.h"

TaskHandle_t xPeriodicGenerator;

void vPeriodicTask(void *pvParamaters);
void vPeriodicGenerator(void *pvParamaters);


#endif /* DD_AUX_H_ */
