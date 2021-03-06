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
//#include "semphr.h"
//#include "portasm.h"

/* Demo includes. */
#include "basic_io_avr.h"

/* Compiler includes. */
//#include <dos.h>

/* 任务函数. */
static void vHandlerTask( void *pvParameters );
static void vPeriodicTask( void *pvParameters );

/* 中断服务程序，此中断发生后，任务将同步运行
The service routine for the interrupt.  This is the interrupt that the task
will be synchronized with. */
static void vExampleInterruptHandler( void );

/*-----------------------------------------------------------*/

/* 定义一个信号量，用于同步运行任务，在中断之后
Declare a variable of type SemaphoreHandle_t.  This is used to reference the
semaphore that is used to synchronize a task with an interrupt. */
SemaphoreHandle_t xCountingSemaphore;

// pin to generate interrupts
#if defined(CORE_TEENSY)
const uint8_t interruptPin = 0;
#elfif defined(__AVR_ATmega32U4__)
const uint8_t interruptPin = 3;
#else  // interruptPin
const uint8_t interruptPin = 2;
#endif  // interruptPin

void setup( void )
{
  Serial.begin(9600);
  /* 创建一个信号量， 最大是10,最小是0
  Before a semaphore is used it must be explicitly created.  In this example
  a counting semaphore is created.  The semaphore is created to have a maximum
  count value of 10, and an initial count value of 0. */
  xCountingSemaphore = xSemaphoreCreateCounting( 10, 0 );

  /* 安装中断服务
    Install the interrupt handler. */
//  _dos_setvect( 0x82, vExampleInterruptHandler );

   pinMode(interruptPin, OUTPUT);
   attachInterrupt(0, vExampleInterruptHandler, RISING);

  /* 如果信号量被建立。
   Check the semaphore was created successfully. */
  if( xCountingSemaphore != NULL )
  {
    /* 建立一个handler任务， 将与中断同步运行，因为它有最高的优先级3
    Create the 'handler' task.  This is the task that will be synchronized
    with the interrupt.  The handler task is created with a high priority to
    ensure it runs immediately after the interrupt exits.  In this case a
    priority of 3 is chosen. */
    xTaskCreate( vHandlerTask, "Handler", 200, NULL, 3, NULL );

    /* 建立Periodic任务  定期用软件产生中断任务。优先级低于handle任务，以确保handle任务获得抢占阻塞状态。
    This is created with a priority below the handler task to ensure it will
    get preempted each time the handler task exist the Blocked state. */
    xTaskCreate( vPeriodicTask, "Periodic", 200, NULL, 1, NULL );

    /* 开始调度Start the scheduler so the created tasks start executing. */
    vTaskStartScheduler();
  }

    /* 出错， 堆栈不够。
    If all is well we will never reach here as the scheduler will now be
    running the tasks.  If we do reach here then it is likely that there was
    insufficient heap memory available for a resource to be created. */
  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

static void vHandlerTask( void *pvParameters )
{
  /* 无限循环
  As per most tasks, this task is implemented within an infinite loop. */
  for( ;; )
  {
    /* 使用信号量等待中断发生。函数阻塞， 函数返回就是已经发生，所以不用判断返回值
    Use the semaphore to wait for the event.  The semaphore was created
    before the scheduler was started so before this task ran for the first
    time.  The task blocks indefinitely meaning this function call will only
    return once the semaphore has been successfully obtained - so there is no
    need to check the returned value. */
    xSemaphoreTake( xCountingSemaphore, portMAX_DELAY );

    /* 中断事件已经发生，在这里可以处理事件，但我们选择只打印一个消息。
    To get here the event must have occurred.  Process the event (in this
    case we just print out a message). */
    vPrintString( "Handler task - Processing event.\r\n" );
  }
}
/*-----------------------------------------------------------*/

static void vPeriodicTask( void *pvParameters )
{
  /* As per most tasks, this task is implemented within an infinite loop. */
  for( ;; )
  {
    /* 通过周期性的置中断腿的电平， 来模拟一个中断
    This task is just used to 'simulate' an interrupt.  This is done by
    periodically generating a software interrupt. */
    vTaskDelay( 500 / portTICK_PERIOD_MS );

    /* Generate the interrupt, printing a message both before hand and
    afterwards so the sequence of execution is evident from the output. */
    vPrintString( "Perodic task - About to generate an interrupt.\r\n" );
//    __asm{ int 0x82 }
    digitalWrite(interruptPin, LOW);
    digitalWrite(interruptPin, HIGH);

    vPrintString( "Periodic task - Interrupt generated.\r\n\r\n\r\n" );
  }
}
/*-----------------------------------------------------------*/

static void vExampleInterruptHandler( void )
{
static portBASE_TYPE xHigherPriorityTaskWoken;

  xHigherPriorityTaskWoken = pdFALSE;

  /* '给'信号， 让任务解除阻塞状态  下面给3次信号， 是为了演示每次事件都不会丢失。
  'Give' the semaphore multiple times.  The first will unblock the handler
  task, the following 'gives' are to demonstrate that the semaphore latches
  the events to allow the handler task to process them in turn without any
  events getting lost.  This simulates multiple interrupts being taken by the
  processor, even though in this case the events are simulated within a single
  interrupt occurrence.*/
  xSemaphoreGiveFromISR( xCountingSemaphore, (BaseType_t*)&xHigherPriorityTaskWoken );
  xSemaphoreGiveFromISR( xCountingSemaphore, (BaseType_t*)&xHigherPriorityTaskWoken );
  xSemaphoreGiveFromISR( xCountingSemaphore, (BaseType_t*)&xHigherPriorityTaskWoken );

  if( xHigherPriorityTaskWoken == pdTRUE )
  {
    /* Giving the semaphore unblocked a task, and the priority of the
    unblocked task is higher than the currently running task - force
    a context switch to ensure that the interrupt returns directly to
    the unblocked (higher priority) task.

    NOTE: The syntax for forcing a context switch is different depending
    on the port being used.  Refer to the examples for the port you are
    using for the correct method to use! */
    // portSWITCH_CONTEXT();
    vPortYield();
  }
}
//---------------------------------------------------------------
void loop() {}
