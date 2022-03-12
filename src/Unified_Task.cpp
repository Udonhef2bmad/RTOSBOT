#include "Unified_Task.h"

#include <Arduino.h>

/// check an update deadline
bool hasDeadlinePassed(TickType_t StartDeadline, TickType_t Deadline)
{
    Serial.printf("Task took %u ms to execute\n", (xTaskGetTickCount() - StartDeadline)*portTICK_PERIOD_MS);
    Serial.printf("Deadline is %u ms behind\n", (Deadline - (xTaskGetTickCount() - StartDeadline))*portTICK_PERIOD_MS);
    
    if (xTaskGetTickCount() - StartDeadline < Deadline)//safe comparison
    {
        return false;
    }
    return true;
}

void taskLoop(void *pvParameters)
{
    // Cast class (came up with that, no idea if this is good practice)
    struct Unified_Task *uTask = (struct Unified_Task *)pvParameters;
    struct Task_Constraints cnst = uTask->cnst;

    TickType_t DeadlineStart; /// keep track of periodicity for vTaskDelayUntil
    TickType_t xLastPeriod;/// keep track of deadline start
    // Wait for first activation

    vTaskDelay(cnst.xFirst);
    Serial.printf("task active\n");

    // Initialise the xLastPeriod variable with the current time.
    xLastPeriod = xTaskGetTickCount();
    while (1)
    {
        // Reset deadline
        DeadlineStart = xTaskGetTickCount();

        // Run looped function
        Serial.printf("executing task\n");
        (*uTask).func(uTask->pvParameters);

        // Check for deadline
        Serial.printf("DeadlineStart : %d\n", DeadlineStart);
        if (hasDeadlinePassed(DeadlineStart, cnst.xDeadline))
        {
            break; // Deadline has arrived. May god have mercy
        }

        // wait for period
        vTaskDelayUntil(&xLastPeriod, cnst.xPeriod); 
    }

    while (1)
    {
        // error handler
        Serial.println("error");
        delay(1000);
    }
}
