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
#include <limits.h>
#include <string.h>

#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

/* --------------- BUILD CONFIGS ----------------------*/

#define ENABLE										(1)
#define DISABLE										(0)
#define MODE_ENABLED(__m__)			((__m__) == ENABLE)
#define MODE_DISABLED(__m__)		((__m__) == DISABLE)


#define DEBUG_MODE								(0)
#define MODE_USE_SAFE_PRINT						(1)
#define DEBUG_LIST								(0)
#define DEBUG_SCHED_CORE						(0)
#define DEBUG_SCHED_CALL						(0)


/* --------------- USEFUL TYPEDEFS --------------------*/

typedef		uint8_t		u8;
typedef		uint16_t	u16;
typedef		uint32_t	u32;
typedef		int8_t		i8;
typedef		int16_t		i16;
typedef		int32_t		i32;

/* --------------- USEFUL DEFINES --------------------*/

#if MODE_ENABLED(DEBUG_MODE)
	#define DEBUG_ONLY(__cmd__)		do{ __cmd__; }while(0)
#else
	#define DEBUG_ONLY(__cmd__)
#endif

#endif /* CONFIG_H_ */








