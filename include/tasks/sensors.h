#ifndef SENSORTASK_H
#define SENSORTASK_H

#include "Unified_Task.h"
#include <Arduino.h>

struct Unified_Task *InitSensorTask(
    bool *s_SensorReady, SemaphoreHandle_t m_SensorReady,
    unsigned long *s_DistanceArray, SemaphoreHandle_t m_DistanceArray,
    char *s_DisplayContent, SemaphoreHandle_t m_DisplayContent);

#endif