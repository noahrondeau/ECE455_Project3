/*
 * DD_SafePrint.c
 *
 *  Created on: Mar 23, 2019
 *      Author: ntron
 */

#include "config.h"
#include "SafePrint.h"
#include <stdarg.h>

#if !defined(MODE_USE_SAFE_PRINT)
#define MODE_USE_SAFE_PRINT		(DISABLE)		// default to not using safe print, must include symbol in project
#endif

#if MODE_ENABLED(MODE_USE_SAFE_PRINT)
static SemaphoreHandle_t xStdoutMutex = NULL;
#endif


void SafePrintInit(void)
{
#if MODE_ENABLED(MODE_USE_SAFE_PRINT)

	xStdoutMutex = xSemaphoreCreateMutex();
	/*
	if (xStdoutMutex == NULL)
		printf("xStdoutMutex could not be allocated\n");

	else
		printf("xStdoutMutex successfully created\n");

	fflush(stdout);
	*/
#endif
}

void SafePrint(bool cond, const char* format, ...)
{
#if MODE_ENABLED(MODE_USE_SAFE_PRINT)
	if (cond)
	{
		xSemaphoreTake(xStdoutMutex, portMAX_DELAY);
		va_list argptr;
		va_start(argptr, format);
		vprintf(format, argptr);
		va_end(argptr);

		fflush(stdout);
		xSemaphoreGive(xStdoutMutex);
	}
#endif
}

void __SafePrintFromTask__(const char* func, bool cond, const char* format, ...)
{
#if MODE_ENABLED(MODE_USE_SAFE_PRINT)
	if (cond)
	{
		char* sCallingTaskName = pcTaskGetName(xTaskGetCurrentTaskHandle());
		TickType_t xTime = xTaskGetTickCount();

		xSemaphoreTake(xStdoutMutex, portMAX_DELAY);

		printf("%s\t(%s)\t(t=%d):\t", sCallingTaskName, func, (i32)xTime);

		va_list argptr;
		va_start(argptr, format);
		vprintf(format, argptr);
		va_end(argptr);
		fflush(stdout);
		xSemaphoreGive(xStdoutMutex);
	}
#endif
}
