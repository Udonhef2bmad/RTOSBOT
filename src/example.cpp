#include "example.h"


struct Param
{
    int test;
};

void func(void *pvParameters)
{
    struct Param *param = (struct Param *)pvParameters;

    Serial.printf("Test func : %d\n", param->test);
    Serial.printf("vATaskFunction %d\n", xPortGetCoreID());
}

struct Unified_Task *construct()
{
    struct Unified_Task *uTask = (struct Unified_Task *)malloc(sizeof *uTask);
    uTask->func = func;

    struct Param *param = (struct Param *)malloc(sizeof *param);
    param->test = 10;
    uTask->pvParameters = param;

    uTask->cnst.xFirst = 0;    /// [O] First activation
    uTask->cnst.xWcet = 0;     /// [C] Worst-case execution time //unused?//
    uTask->cnst.xPeriod = 500 / portTICK_PERIOD_MS;   /// [T] Period
    uTask->cnst.xDeadline = 500 / portTICK_PERIOD_MS; /// [D] Deadline

    uTask->vars.name = "DefaultName";
    uTask->vars.usStackDepth = 1000;
    uTask->vars.pvParameters = (void *)1;
    uTask->vars.uxPriority = 1;
    uTask->vars.pvCreatedTask = NULL;
    return uTask;
}
