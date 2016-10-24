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

/* 任务函数. */
void vContinuousProcessingTask( void *pvParameters );
void vPeriodicTask( void *pvParameters );

/* 定义传递给任务的参数， 要定义为const模式,不使用堆栈，以便任务能够正确的运行。*/
const char *pcTextForTask1 = "Continuous task 1 running\r\n";
const char *pcTextForTask2 = "Continuous task 2 running\r\n";
const char *pcTextForPeriodicTask = "Periodic task is running\r\n";

/*-----------------------------------------------------------*/

void setup( void )
{
  Serial.begin(9600);
  /* Create two instances of the continuous processing task, both at priority 1.
     建立2个连续任务，优先级都是1 */
  xTaskCreate( vContinuousProcessingTask, "Task 1", 200, (void*)pcTextForTask1, 1, NULL );
  xTaskCreate( vContinuousProcessingTask, "Task 2", 200, (void*)pcTextForTask2, 1, NULL );

  /* 建立1个周期任务 优先级2. */
  xTaskCreate( vPeriodicTask, "Task 3", 200, (void*)pcTextForPeriodicTask, 2, NULL );

  /* 开始任务调度. */
  vTaskStartScheduler();

  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

void vContinuousProcessingTask( void *pvParameters )
{
char *pcTaskName;

  /* 参数转换成字符串指针. */
  pcTaskName = ( char * ) pvParameters;

  /* 无限循环. */
  for( ;; )
  {
    /* 持续打印任务名. */
    vPrintString( pcTaskName );
  }
}
/*-----------------------------------------------------------*/

void vPeriodicTask( void *pvParameters )
{
TickType_t xLastWakeTime;

  /*  xLastWakeTime 变量需要用当前tick计数值进行初始化，我们只需要在这里访问这个变量一次，
以后将由API函数 vTaskDelayUntil（）自动管理
  The xLastWakeTime variable needs to be initialized with the current tick
  count.  Note that this is the only time we access this variable.  From this
  point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
  API function. */
  xLastWakeTime = xTaskGetTickCount();

  /* 无限循环. */
  for( ;; )
  {
    /* 打印字符串. */
    vPrintString( "Periodic task is running\r\n" );

    /* 500ms延迟. */
    vTaskDelayUntil( &xLastWakeTime, ( 500 / portTICK_PERIOD_MS ) );
  }
}
//------------------------------------------------------------------------------
void loop() {}
