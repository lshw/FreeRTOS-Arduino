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

/* 2个任务函数. */
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );

/* 用于保存任务2的句柄. */
TaskHandle_t xTask2Handle;

/*-----------------------------------------------------------*/

void setup( void )
{
  Serial.begin(9600);
  /* 创建优先级为2的第一个任务。此时任务参数为未使用(NULL)。 任务句柄也不使用(NULL)。
Create the first task at priority 2.  This time the task parameter is
  not used and is set to NULL.  The task handle is also not used so likewise
  is also set to NULL. */
  xTaskCreate( vTask1, "Task 1", 200, NULL, 2, NULL );
       /* The task is created at priority 2 ^. */

  /* 创建优先级为1的第二个任务 - 优先级低于任务1。 同样，不使用任务参数，因此设置为NULL - 
   但是这次我们要获取任务的句柄，所以传入变量地址 . */
  xTaskCreate( vTask2, "Task 2", 200, NULL, 1, &xTask2Handle );
                            /* 任务句柄是最后一个参数 ^^^^^^^^^^^^^ */

  /* 开始任务调度. */
  vTaskStartScheduler();

  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

void vTask1( void *pvParameters )
{
unsigned portBASE_TYPE uxPriority;

  /* 此任务将始终在Task2之前运行，因为它具有较高的优先级。
   任务1和任务2都不是阻塞的，因此两者都将始终在运行或就绪状态
  This task will always run before Task2 as it has the higher priority.
  Neither Task1 nor Task2 ever block so both will always be in either the
  Running or the Ready state.
  
  传入NULL参数，查询本任务的优先级 
  Query the priority at which this task is running - passing in NULL means
  "return our own priority". */
  uxPriority = uxTaskPriorityGet( NULL );

  for( ;; )
  {
    /* 打印任务名字. */
    vPrintString( "Task1 is running\r\n" );

    /* 
    将Task2优先级设置为高于Task1优先级将导致任务2立即开始运行
（因为任务2在2个任务中有更高的优先级）。
    Setting the Task2 priority above the Task1 priority will cause
    Task2 to immediately start running (as then Task2 will have the higher
    priority of the    two created tasks). */
    vPrintString( "About to raise the Task2 priority\r\n" );
    vTaskPrioritySet( xTask2Handle, ( uxPriority + 1 ) );

    /* Task1只有在其优先级高于Task2时才会运行。
     因此，Task2必须已经执行并将其自身优先级设置为0。
    Task1 will only run when it has a priority higher than Task2.
    Therefore, for this task to reach this point Task2 must already have
    executed and set its priority back down to 0. */
  }
}

/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
unsigned portBASE_TYPE uxPriority;

  /* 此任务将始终在Task2之前运行，因为它具有较高的优先级。
   任务1和任务2都不是阻塞的，因此两者都将始终在运行或就绪状态
  Task1 will always run before this task as Task1 has the higher priority.
  Neither Task1 nor Task2 ever block so will always be in either the
  Running or the Ready state.


   传入NULL参数，查询本任务的优先级 
  Query the priority at which this task is running - passing in NULL means
  "return our own priority". */
  uxPriority = uxTaskPriorityGet( NULL );

  for( ;; )
  {
    /* 要想这个任务运行，Task1必须已经运行和设置Task2的优先级高于Task1。
    For this task to reach this point Task1 must have already run and
    set the priority of this task higher than its own.

    显示任务名
    Print out the name of this task. */
    vPrintString( "Task2 is running\r\n" );

    /* 将优先级设置为原始值， 以便Task1可以运行，传入NULL参数，意味着修改自身任务的优先级

    Set our priority back down to its original value.  Passing in NULL
    as the task handle means "change our own priority".  Setting the
    priority below that of Task1 will cause Task1 to immediately start
    running again. */
    vPrintString( "About to lower the Task2 priority\r\n" );
    vTaskPrioritySet( NULL, ( uxPriority - 2 ) );
  }
}
/*-----------------------------------------------------------*/
void loop() {}


