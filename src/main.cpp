#include <Arduino.h>

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "Unified_Task.h"

#include "modules/displays.h"
#include "modules/sensors.h"
#include "modules/motors.h"


void vATaskFunction(void *taskParam) // <- une tâche
{
    int variable1;        // <- variable allouée dans la pile (*stack*) de la tâche et unique pour chaque instance de tâche
    static int variable2; // <- variable allouée en dehors de la pile de la tâche et partagée pour chaque instance de tâche

    for (;;) // <- boucle infinie
    {
        Serial.printf("vATaskFunction %d\n", xPortGetCoreID());
        delay(1000);
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;
    Serial.println("Start");

    xTaskCreate(
        vATaskFunction,   /* Task function. */
        "vATaskFunction", /* name of task. */
        1000,             /* Stack size of task */
        NULL,             /* parameter of the task */
        1,                /* priority of the task */
        NULL);            /* Task handle to keep track of created task */
}

void loop()
{
    Serial.printf("Task loop() %d\n", xPortGetCoreID());
    delay(1000);
}