#include "Unified_Task.h"

#include <Arduino.h>

bool DEBUG = false; // hide task details [not good practice, but time is of the essence]

/// check an update deadline
bool hasDeadlinePassed(TickType_t StartDeadline, TickType_t Deadline)
{
    if (DEBUG)
    {
        Serial.print("Task took ");
        Serial.print((xTaskGetTickCount() - StartDeadline) * portTICK_PERIOD_MS);
        Serial.println("ms to execute");
        Serial.print("Deadline is ");
        Serial.print((Deadline - (xTaskGetTickCount() - StartDeadline)) * portTICK_PERIOD_MS);
        Serial.println("ms behind");
    }

    if (xTaskGetTickCount() - StartDeadline < Deadline) // safe comparison
    {
        return false;
    }
    return true;
}

void taskLoop(void *pvParameters)
{
    // Cast class (came up with that, no idea if this is good practice)
    struct Unified_Task *uTask = (struct Unified_Task *)pvParameters;
    struct Task_Constraints *cnst = uTask->cnst;

    TickType_t DeadlineStart; /// keep track of periodicity for vTaskDelayUntil
    TickType_t xLastPeriod;   /// keep track of deadline start

    // run setup function once
    if (DEBUG)
        Serial.print("Task initialised : Running setup\n");
    (*uTask).uSetup(uTask->param);

    // Wait for first activation
    vTaskDelay((*cnst).xFirst);
    if (DEBUG)
        Serial.print("Task Active : Running loop\n");

    // Initialise the xLastPeriod variable with the current time.
    xLastPeriod = xTaskGetTickCount();
    while (1)
    {
        // Reset deadline
        DeadlineStart = xTaskGetTickCount();

        // Run looped function
        if (DEBUG)
            Serial.print("\nExecuting task\n");
        (*uTask).uLoop(uTask->param);
        if (DEBUG)
            Serial.print("Done executing task\n");

        // stack size check (use are the "tip" of the function to check limits)
        // uxTaskGetStackHighWaterMark(NULL);

        // Check for deadline
        if (DEBUG)
            Serial.print("DeadlineStart :");
        if (DEBUG)
            Serial.println(DeadlineStart);
        if (hasDeadlinePassed(DeadlineStart, (*cnst).xDeadline))
        {
            break; // Deadline has arrived. May god have mercy
        }

        // wait for period
        vTaskDelayUntil(&xLastPeriod, (*cnst).xPeriod);
    }

    while (1)
    {
        // error handler
        if (DEBUG)
            Serial.println("error");
        delay(1000);
    }
}

void CreateTask(struct Unified_Task *uTask)
{
    xTaskCreate(
        taskLoop,                    // Task function.
        uTask->info->name,           // name of task.
        uTask->info->usStackDepth,   // Stack size of task
        uTask,                       // parameter of the task
        uTask->info->uxPriority,     // priority of the task
        uTask->info->pvCreatedTask); // Task handle to keep track of created task
}