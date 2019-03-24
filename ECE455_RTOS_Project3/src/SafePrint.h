/*
 * DD_SafePrint.h
 *
 *  Created on: Mar 23, 2019
 *      Author: ntron
 */

#ifndef SAFEPRINT_H_
#define SAFEPRINT_H_

#include <stdbool.h>

void SafePrintInit(void);
void SafePrint(bool cond, const char* format, ...);
void __SafePrintFromTask__(const char* func,bool cond, const char* format, ...);

#define SafePrintFromTask(...)			__SafePrintFromTask__(__func__, __VA_ARGS__)
#define DebugSafePrint(...)		__SafePrintFromTask__(__func__, DEBUG_MODE, __VA_ARGS__)


#endif /* SAFEPRINT_H_ */
