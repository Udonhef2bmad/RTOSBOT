#ifndef COMMANDTASK_H
#define COMMANDTASK_H

#include "Unified_Task.h"
#include <Arduino.h>

// there's gotta be a better way to do this, but i'm out of time and
struct Unified_Task *InitControlTask(
    bool *s_SensorReady, SemaphoreHandle_t m_SensorReady,
    unsigned long *s_DistanceArray, SemaphoreHandle_t m_DistanceArray,
    uint32_t *s_MotorOutputs, SemaphoreHandle_t m_MotorOutputs,
    char *s_DisplayContent, SemaphoreHandle_t m_DisplayContent);

#endif