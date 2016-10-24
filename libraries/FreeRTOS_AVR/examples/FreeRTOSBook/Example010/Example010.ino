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
//#include "queue.h"

/* Demo includes. */
#include "basic_io_avr.h"


/* 要被创建的Task。2个发送任务的实例，一个接收任务的实例
The tasks to be created.  Two instances are created of the sender task while
only a single instance is created of the receiver task. */
static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* 定义一个变量，用于存储队列句柄，3个任务都可以访问它
Declare a variable of type QueueHandle_t.  This is used to store the queue
that is accessed by all three tasks. */
QueueHandle_t xQueue;


void setup( void )
{
  Serial.begin(9600);
    /* 建立一个5个长整型的队列 */
   /* The queue is created to hold a maximum of 5 long values. */
    xQueue = xQueueCreate( 5, sizeof( long ) );

  if( xQueue != NULL )
  {
    /* 创建2个优先级都是1的任务，用来向队列写入数据，一个写100,一个写200
     Create two instances of the task that will write to the queue.  The
    parameter is used to pass the value that the task should write to the queue,
    so one task will continuously write 100 to the queue while the other task
    will continuously write 200 to the queue.  Both tasks are created at
    priority 1. */
    xTaskCreate( vSenderTask, "Sender1", 200, ( void * ) 100, 1, NULL );
    xTaskCreate( vSenderTask, "Sender2", 200, ( void * ) 200, 1, NULL );

    /* 建立一个任务，从队列读数据，优先级=2,高于2个发送任务的优先级
    Create the task that will read from the queue.  The task is created with
    priority 2, so above the priority of the sender tasks. */
    xTaskCreate( vReceiverTask, "Receiver", 200, NULL, 2, NULL );

    /* 开始调度. */
    vTaskStartScheduler();
  }
  else
  {
    /* 队列不能被建立 。 The queue could not be created. */
  }

    /* 一般不会运行到这里，除非堆栈不够。
    If all is well we will never reach here as the scheduler will now be
    running the tasks.  If we do reach here then it is likely that there was
    insufficient heap memory available for a resource to be created. */
  for( ;; );
//  return 0;
}
/*-----------------------------------------------------------*/

static void vSenderTask( void *pvParameters )
{
long lValueToSend;
portBASE_TYPE xStatus;

  /* 建立2个发送任务，用参数来设置发送到队列的值，因此，这里需要对参数进行格式转换
  Two instances are created of this task so the value that is sent to the
  queue is passed in via the task parameter rather than be hard coded.  This way
  each instance can use a different value.  Cast the parameter to the required
  type. */
  lValueToSend = ( long ) pvParameters;

  /* 无限循环. */
  for( ;; )
  {
    /* 第一个参数时队列句柄
    The first parameter is the queue to which data is being sent.  The
    queue was created before the scheduler was started, so before this task
    started to execute.
     
     第二个参数是要发送的值的地址
    The second parameter is the address of the data to be sent.
     
     
    第三个参数是阻塞时间，如果队列满了，需要等待的时间， 在本例子中，不可能满，所以
    我们置为0,
    The third parameter is the Block time, the time the task should be kept
    in the Blocked state to wait for space to become available on the queue
    should the queue already be full.  In this case we don't specify a block
    time because there should always be space in the queue. */
    xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );

    if( xStatus != pdPASS )
    {
      /* 队列满了，我们无法写入，这里肯定有问题， 
      We could not write to the queue because it was full, this must
      be an error as the queue should never contain more than one item! */
      vPrintString( "Could not send to the queue.\r\n" );
    }

    /* 主动释放时间片，让发送Task有机会运行。Allow the other sender task to execute. */
    taskYIELD();
  }
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
/* 保存从对列中取出的数据。Declare the variable that will hold the values received from the queue. */
long lReceivedValue;
portBASE_TYPE xStatus;
const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;

  /* 死循环. */
  for( ;; )
  {
    /* 非阻塞方式查询队列数据.As this task unblocks immediately that data is written to the queue this
    call should always find the queue empty. */
    if( uxQueueMessagesWaiting( xQueue ) != 0 )
    {
      vPrintString( "Queue should have been empty!\r\n" );
    }

    /* 第一个参数是队列句柄。
    The first parameter is the queue from which data is to be received.  The
    queue is created before the scheduler is started, and therefore before this
    task runs for the first time.

    第二个参数是保存数据的地址
    The second parameter is the buffer into which the received data will be
    placed.  In this case the buffer is simply the address of a variable that
    has the required size to hold the received data.

     最后一个参数时阻塞时间。
    the last parameter is the block time, the maximum amount of time that the
    task should remain in the Blocked state to wait for data to be available should
    the queue already be empty. */
    xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );

    if( xStatus == pdPASS )
    {
      /* 完成取数。
      Data was successfully received from the queue, print out the received
      value. */
      vPrintStringAndNumber( "Received = ", lReceivedValue );
    }
    else
    {
      /* 不可能取不到数，出错了。
      We did not receive anything from the queue even after waiting for 100ms.
      This must be an error as the sending tasks are free running and will be
      continuously writing to the queue. */
      vPrintString( "Could not receive from the queue.\r\n" );
    }
  }
}
//------------------------------------------------------------------------------
void loop() {}
