#ifndef MOTORTASK_H
#define MOTORTASK_H

#include "Unified_Task.h"
#include <Arduino.h>

struct Unified_Task *InitMotorTask(
    uint32_t *s_MotorOutputs, SemaphoreHandle_t m_MotorOutputs,
    char *s_DisplayContent, SemaphoreHandle_t m_DisplayContent);

#endif