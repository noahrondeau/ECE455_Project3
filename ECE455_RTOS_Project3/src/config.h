/*
 * config.h
 *
 *  Created on: Mar 14, 2019
 *      Author: ntron
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* ----------- Includes --------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4_discovery.h"

#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

/* --------------- USEFUL DEFINES --------------------*/

typedef		uint8_t		u8;
typedef		uint16_t	u16;
typedef		uint32_t	u32;
typedef		int8_t		i8;
typedef		int16_t		i16;
typedef		int32_t		i32;

#endif /* CONFIG_H_ */
