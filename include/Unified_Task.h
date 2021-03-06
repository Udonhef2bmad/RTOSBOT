#ifndef UNIFIED_TASK_H
#define UNIFIED_TASK_H


#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"




struct Task_Information
{
    const char *name = "DefaultName";
    uint32_t usStackDepth = 64;
    void *pvParameters = (void *)1;
    UBaseType_t uxPriority = tskIDLE_PRIORITY;
    TaskHandle_t *pvCreatedTask = NULL;
};

struct Task_Constraints
{
    /// following time constraints apply to task function
    TickType_t xFirst;    // [O] First activation
    TickType_t xWcet;     // [C] Worst-case execution time //unused?//
    TickType_t xPeriod;   // [T] Period
    TickType_t xDeadline; // [D] Deadline
};

struct Unified_Task
{
    struct Task_Constraints *cnst;
    struct Task_Information *info;
    void (*uSetup)(void *);//called once before loop
    void (*uLoop)(void *);//loops infinitely
    void *param;
};

//void taskLoop(void *pvParameters);
void CreateTask(struct Unified_Task *uTask);

#endif