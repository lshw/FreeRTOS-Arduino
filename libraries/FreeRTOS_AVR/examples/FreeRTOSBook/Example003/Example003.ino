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
//#include "FreeRTOS.h"
//#include "task.h"
#include "FreeRTOS_AVR.h"

/* Demo includes. */
#include "basic_io_avr.h"

/* 使用loop计数， 建立一个粗略的延迟 . */
#define mainDELAY_LOOP_COUNT    400000 //( 0xffffff )

/* 任务函数. */
void vTaskFunction( void *pvParameters );

/* 定义传递给任务的参数， 要定义为const模式,不使用堆栈，以便任务能够正确的运行。 */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\t\n";

/*-----------------------------------------------------------*/

void setup( void )
{
  Serial.begin(9600);
  /* 建立第一个任务，优先级=1... */
  xTaskCreate( vTaskFunction, "Task 1", 200, (void*)pcTextForTask1, 1, NULL );

  /* ..第二个任务，优先级=2.  优先级是倒数第二个参数. */
  xTaskCreate( vTaskFunction, "Task 2", 200, (void*)pcTextForTask2, 2, NULL );

    /* 启动调度程序，使我们的任务开始执行. */
  vTaskStartScheduler();

  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

void vTaskFunction( void *pvParameters )
{
char *pcTaskName;
volatile unsigned long ul;

  /*要打印的字符串通过参数传入。 把这个转换成字符指针。. */
  pcTaskName = ( char * ) pvParameters;

    /* 大部分的任务是无限循环的. */
  for( ;; )
  {
    /*  打印出任务名字. */
    vPrintString( pcTaskName );

    /* 延迟一会儿. */
    for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
    {
      /* 一个简单的循环， 以后，我们将用 delay/sleep 来实现. */
    }
  }
}
//-----------------------------------------------------------------
void loop() {}

