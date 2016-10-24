/*
  FreeRTOS.org V5.0.4 - Copyright (C) 2003-2008 Richard Barry.

  This file is part of the FreeRTOS.org distribution.

  FreeRTOS.org is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  FreeRTOS.org is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FreeRTOS.org; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  A special exception to the GPL can be applied should you wish to distribute
  a combined work that includes FreeRTOS.org, without being obliged to provide
  the source code for any proprietary components.  See the licensing section
  of http://www.FreeRTOS.org for full details of how and when the exception
  can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

  Please ensure to read the configuration and relevant port sections of the
  online documentation.

  http://www.FreeRTOS.org - Documentation, latest information, license and
  contact details.

  http://www.SafeRTOS.com - A version that is certified for use in safety
  critical systems.

  http://www.OpenRTOS.com - Commercial support, development, porting,
  licensing and training services.
*/

/* FreeRTOS.org includes. */
#include "FreeRTOS_AVR.h"
//#include "task.h"

/* Demo includes. */
#include "basic_io_avr.h"

/* The 任务函数. */
void vTaskFunction( void *pvParameters );

/* 定义传递给任务的参数， 要定义为const模式,不使用堆栈，以便任务能够正确的运行。 */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\t\n";

/*-----------------------------------------------------------*/

void setup( void )
{
  Serial.begin(9600);
  /* 建立第一个任务 优先级=1... */
  xTaskCreate( vTaskFunction, "Task 1", 200, (void*)pcTextForTask1, 1, NULL );

  /* 建立第二个任务，优先级=2. */
  xTaskCreate( vTaskFunction, "Task 2", 200, (void*)pcTextForTask2, 2, NULL );

  /* 开始运行调度程序. */
  vTaskStartScheduler();

  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

void vTaskFunction( void *pvParameters )
{
char *pcTaskName;
TickType_t xLastWakeTime;

  /* 参数转换成字符串指针. */
  pcTaskName = ( char * ) pvParameters;

  /*  xLastWakeTime 变量需要用当前tick计数值进行初始化，我们只需要在这里访问这个变量一次，
以后将由API函数 vTaskDelayUntil（）自动管理
    The xLastWakeTime variable needs to be initialized with the current tick
  count.  Note that this is the only time we access this variable.  From this
  point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
  API function */
  xLastWakeTime = xTaskGetTickCount();

  /* 无限循环. */
  for( ;; )
  {
    /* 打印任务名. */
    vPrintString( pcTaskName );
   /*我们希望此任务每250毫秒执行一次。 按照
     vTaskDelay（）函数，时间以ticks为单位测量，而
     portTICK_PERIOD_MS常量用于将其转换为毫秒。
     xLastWakeTime在vTaskDelayUntil（）中自动更新，因此不会
     必须通过此任务代码更新。
    We want this task to execute exactly every 250 milliseconds.  As per
    the vTaskDelay() function, time is measured in ticks, and the
    portTICK_PERIOD_MS constant is used to convert this to milliseconds.
    xLastWakeTime is automatically updated within vTaskDelayUntil() so does not
    have to be updated by this task code. 
     */

    vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_PERIOD_MS ) );
  }
}
//------------------------------------------------------------------------------
void loop() {}


