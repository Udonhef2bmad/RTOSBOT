#include "Unified_Task.h"

Unified_Task::Unified_Task()
{
}

void Unified_Task::SetTickParams(TickType_t first, TickType_t wcet, TickType_t period, TickType_t deadline)
{
}

// From what i understand, you can't cast a function member to a function pointer,
// so i'm passing the class as a parameteer as a workaround.
void taskLoop(void *pvParameters)
{
    // Cast class (came up with that, no idea if this is good practice)
    Unified_Task *uTask = static_cast<Unified_Task *>(pvParameters);

    // Wait for first activation
    vTaskDelay((*uTask).xFirst);

    // Initialise the xLastPeriod variable with the current time.
    (*uTask).xLastPeriod = xTaskGetTickCount();
    while (1)
    {
        // Reset deadline
        (*uTask).xStartDeadline = xTaskGetTickCount();

        // Run looped function
        (*uTask).uLoop();

        // Check for deadline
        if ((*uTask).xStartDeadline > (*uTask).xDeadline)
        {
            break; // Deadline has arrived. May god have mercy
        }

        vTaskDelayUntil(&(*uTask).xLastPeriod, (*uTask).xPeriod); // wait for period
    }

    while (1)
    {
        // error handler
    }
}

void Unified_Task::create()
{
    // void (*loop_ptr)(void *pvParameters) = loop;
    xTaskCreate(
        taskLoop,     /* Function that implements the task. */
        name,         /* Text name for the task. */
        usStackDepth, /* Stack size in words, not bytes. */
        this,         /* Parameter passed into the task. */
        uxPriority,   /* Priority at which the task is created. */
        pvCreatedTask);
}