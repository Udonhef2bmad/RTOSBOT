#include <Arduino.h>

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "tasks/motors.h"
#include "tasks/sensors.h"
#include "tasks/example.h"


#include "Unified_Task.h"

//#include "modules/displays.h"
//#include "modules/sensors.h"
//#include "modules/motors.h"

void vATaskFunction(void *taskParam) // <- une tâche
{
    //int variable1;        // <- variable allouée dans la pile (*stack*) de la tâche et unique pour chaque instance de tâche
    //static int variable2; // <- variable allouée en dehors de la pile de la tâche et partagée pour chaque instance de tâche

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
    delay(3000);
    //struct Unified_Task *uTask = InitSensorTask();
    struct Unified_Task *uTask = InitMotorTask();
    Serial.println("Done");
    /*
    Serial.printf("name %s\n", uTask->info->name);
    Serial.printf("usStackDepth %d\n", uTask->info->usStackDepth);
    Serial.printf("uxPriority %d\n", uTask->info->uxPriority);*/

    CreateTask(uTask);
}

void loop()
{
    Serial.printf("Task loop() %d\n", xPortGetCoreID());
    delay(10000);
}