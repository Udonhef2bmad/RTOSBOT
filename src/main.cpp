#include <Arduino.h>

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "tasks/control.h"
#include "tasks/motors.h"
#include "tasks/sensors.h"
#include "tasks/example.h"

#include "Unified_Task.h"

//#include "modules/displays.h"
//#include "modules/sensors.h"
//#include "modules/motors.h"

void vATaskFunction(void *taskParam) // <- une tâche
{
    // int variable1;        // <- variable allouée dans la pile (*stack*) de la tâche et unique pour chaque instance de tâche
    // static int variable2; // <- variable allouée en dehors de la pile de la tâche et partagée pour chaque instance de tâche

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


    //// shared between SensorTask and ControlTask
    bool s_SensorReady;                // shared variable that signals compute task to run
    unsigned long s_DistanceArray[3];  // shared variable holding sensor distances
    SemaphoreHandle_t m_SensorReady;   // mutex for s_SensorReady
    SemaphoreHandle_t m_DistanceArray; // mutex for s_distance_array
    m_SensorReady = xSemaphoreCreateMutex();
    m_DistanceArray = xSemaphoreCreateMutex();

    // shared between MotorTask and ControlTask
    uint32_t s_MotorOutputs[2];       // dual uint32_t array that holds dual motor output speeds
    SemaphoreHandle_t m_MotorOutputs; // mutex for s_MotorOutputs
    m_MotorOutputs = xSemaphoreCreateMutex();

    // shared between DisplayTask and ?
    char s_DisplayContent[1000];        // array that holds logging info to be displayed
    SemaphoreHandle_t m_DisplayContent; // mutex for m_DisplayContent
    m_DisplayContent = xSemaphoreCreateMutex();

    struct Unified_Task *uTask;

    uTask = InitSensorTask(
        &s_SensorReady, m_SensorReady,
        s_DistanceArray, m_DistanceArray,
        s_DisplayContent, m_DisplayContent);
    CreateTask(uTask); //*/

    uTask = InitMotorTask(
        s_MotorOutputs, m_MotorOutputs,
        s_DisplayContent, m_DisplayContent);
    CreateTask(uTask); //*/

    uTask = InitControlTask(
        &s_SensorReady, m_SensorReady,
        s_DistanceArray, m_DistanceArray,
        s_MotorOutputs, m_MotorOutputs,
        s_DisplayContent, m_DisplayContent);
    CreateTask(uTask); //*/

    Serial.println("Done");
}

void loop()
{
    // Serial.printf("Task loop() %d\n", xPortGetCoreID());
    delay(10000);
}