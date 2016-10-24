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

/* 2个Task函数. */
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );

/* 用于保存Task2的句柄. */
TaskHandle_t xTask2Handle;

/*-----------------------------------------------------------*/

void setup( void )
{
  // Insure malloc works in tasks
  //确保 malloc 在tasks中可以工作
  __malloc_heap_end = (char*)RAMEND;
  Serial.begin(9600);
  /* 建立第一个任务，优先级=1 不使用句柄. */
  xTaskCreate( vTask1, "Task 1", 200, NULL, 1, NULL );
          /* The task is created at priority 1 ^. */

  /* 开始调度. */
  vTaskStartScheduler();

  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

void vTask1( void *pvParameters )
{
const TickType_t xDelay100ms = 100 / portTICK_PERIOD_MS;

  for( ;; )
  {
    /* 打印任务名. */
    vPrintString( "Task1 is running\r\n" );

    /* 以更高的优先级创建任务2。 任务参数无 ，但是这次我们要在保存任务句柄到变量xTask2Handle所在的地址

    Create task 2 at a higher priority.  Again the task parameter is not
    used so is set to NULL - BUT this time we want to obtain a handle to the
    task so pass in the address of the xTask2Handle variable. */
    xTaskCreate( vTask2, "Task 2", 200, NULL, 2, &xTask2Handle );
       /* The task handle is the last parameter ^^^^^^^^^^^^^ */

    /* 新建的Task2具有更高的优先级，因此，要想回到Task1,Task2必须被运行， 并删除自身Task2

    延迟100ms;
    Task2 has/had the higher priority, so for Task1 to reach here Task2
    must have already executed and deleted itself.  Delay for 100
    milliseconds. */
    vTaskDelay( xDelay100ms );
  }
}

/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
  /*
  Task2 除了删除自己以外啥也不干，它只需要用NULL做参数就可以删除自己，用xTask2Handle做参数， 
   纯粹为了演示目的
  Task2 does nothing but delete itself.  To do this it could call vTaskDelete()
  using a NULL parameter, but instead and purely for demonstration purposes it
  instead calls vTaskDelete() with its own task handle. */
  vPrintString( "Task2 is running and about to delete itself\r\n" );
  vTaskDelete( xTask2Handle );
}
/*-----------------------------------------------------------*/
void loop() {}


