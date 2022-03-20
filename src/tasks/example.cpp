#include "tasks/example.h"

namespace // hidden
{

    struct Param
    {
        int test;
    };

    void getParameters(struct Param *param)
    {
        // set parameters here
        param->test = 10;
    }

    void getConstraints(struct Task_Constraints *cnst)
    {
        // set constraints here
        cnst->xFirst = 0;                           /// [O] First activation
        cnst->xWcet = 0;                            /// [C] Worst-case execution time //unused?//
        cnst->xPeriod = 500 / portTICK_PERIOD_MS;   /// [T] Period
        cnst->xDeadline = 500 / portTICK_PERIOD_MS; /// [D] Deadline
    }

    void getInformation(struct Task_Information *info)
    {
        // set variables here
        info->name = "ExampleTask";
        info->usStackDepth = 1000;
        info->pvParameters = (void *)1;
        info->uxPriority = tskIDLE_PRIORITY;
        info->pvCreatedTask = NULL;
    }

    void uSetup(void *pvParameters)
    {
        // nothing to do
    }

    void uLoop(void *pvParameters)
    {
        struct Param *param = (struct Param *)pvParameters;

        Serial.printf("Test func : %d\n", param->test);
        Serial.printf("vATaskFunction %d\n", xPortGetCoreID());
    }
}

struct Unified_Task *InitExampleTask()
{
    struct Unified_Task *uTask = (struct Unified_Task *)malloc(sizeof *uTask);
    struct Task_Information *info = (struct Task_Information *)malloc(sizeof *info);
    struct Task_Constraints *cnst = (struct Task_Constraints *)malloc(sizeof *cnst);
    struct Param *param = (struct Param *)malloc(sizeof *param);

    // init setup funtion
    uTask->uSetup = uSetup;

    // init looped function
    uTask->uLoop = uLoop;

    // init parameters
    getInformation(info);
    uTask->info = info;
    getConstraints(cnst);
    uTask->cnst = cnst;
    getParameters(param);
    uTask->param = param;

    return uTask;
}