#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#ifndef _UNIFIED_TASK_H
#define _UNIFIED_TASK_H

/// @brief Universal task abstract class. This class must be inherited by a base class.
/// It applies task time constraints to the tasks
class Unified_Task
{
//private:
public:
    /// task creation varaibles
    const char *name;
    uint32_t usStackDepth;
    void *pvParameters;
    UBaseType_t uxPriority;
    TaskHandle_t *pvCreatedTask;

    /// following time constraints apply to task function
    TickType_t xFirst;    /// [O] First activation
    TickType_t xWcet;     /// [C] Worst-case execution time
    TickType_t xPeriod;   /// [T] Period
    TickType_t xDeadline; /// [D] Deadline

    TickType_t xLastPeriod;    /// keep track of periodicity for vTaskDelayUntil
    TickType_t xStartDeadline; /// keep track of deadline start

public:
    Unified_Task();

    // initialisation lists are faster than assignment
    void SetTickParams(TickType_t first, TickType_t wcet, TickType_t period, TickType_t deadline);
    void create();                     // creates the task
    //static void loop( void * pvParameters );                       // loop func with applied time constraints
    virtual TaskFunction_t uLoop() = 0; // defined in base class
    // Destructor
    //~uTask();
};

#endif

/*
class TaskBase {
public:
  xTaskHandle handle;
  ~TaskBase() {
#if INCLUDE_vTaskDelete
    vTaskDelete(handle);
#endif
    return;
  }
};
class Task : public TaskBase {
public:
  Task(char const*name, void (*taskfun)(void *), unsigned portBASE_TYPE priority,
       unsigned portSHORT stackDepth=configMINIMAL_STACK_SIZE) {
    xTaskCreate(taskfun, (signed char*)name, stackDepth, this, priority, &handle);
  }
};
class TaskClass : public TaskBase {
public:
  TaskClass(char const*name, unsigned portBASE_TYPE priority,
           unsigned portSHORT stackDepth=configMINIMAL_STACK_SIZE) {
    xTaskCreate(&taskfun, (signed char*)name, stackDepth, this, priority, &handle);
  }
  virtual void task() = 0;
  static void taskfun(void* parm) {
    static_cast<TaskClass *>(parm)->task();
#if INCLUDE_vTaskDelete
    xTaskDelete(handle);
#else
    while(1)
      vTaskDelay(10000);
#endif
  }
};
*/