#include "tasks/example.h"

namespace // hidden
{
    bool TASKDEBUG = false;

    struct Param
    {
        unsigned long sensor_threshold; // sensor value at which the robot backs off

        bool *s_SensorReady;             // shared variable that signals compute task to run
        SemaphoreHandle_t m_SensorReady; // mutex for s_SensorReady

        unsigned long *s_DistanceArray;    // shared array holding measured distances
        SemaphoreHandle_t m_DistanceArray; // mutex for s_DistanceArray

        uint32_t *s_MotorOutputs;         // dual uint32_t array that holds dual motor output speeds
        SemaphoreHandle_t m_MotorOutputs; // mutex for s_SensorReady

        char *s_DisplayContent;             // array that holds logging info to be displayed
        SemaphoreHandle_t m_DisplayContent; // mutex for s_SensorReady
    };

    void getParameters(struct Param *param)
    {
        // set parameters here
        param->sensor_threshold = 15;
    }

    void getConstraints(struct Task_Constraints *cnst)
    {
        // set constraints here
        cnst->xFirst = 30 / portTICK_PERIOD_MS;    /// [O] First activation
        cnst->xWcet = 5 / portTICK_PERIOD_MS;      /// [C] Worst-case execution time //unused?//
        cnst->xPeriod = 50 / portTICK_PERIOD_MS;   /// [T] Period
        cnst->xDeadline = 50 / portTICK_PERIOD_MS; /// [D] Deadline
    }

    void getInformation(struct Task_Information *info)
    {
        // set variables here
        info->name = "CommandTask";
        info->usStackDepth = 1000;
        info->pvParameters = (void *)1;
        info->uxPriority = 2;
        info->pvCreatedTask = NULL;
    }

    void uSetup(void *pvParameters)
    {
        struct Param *param = (struct Param *)pvParameters;

        // wait for sensor scan
        *(param->s_SensorReady) = false;
    }

    void uLoop(void *pvParameters)
    {

        struct Param *param = (struct Param *)pvParameters;

        if (*(param->s_SensorReady) == true) // sensors are ready, run task
        {
            *(param->s_SensorReady) = false; // reset flag

            if (TASKDEBUG)
            {
                Serial.print("Running Control\n");
                Serial.print("[");
                Serial.print(param->s_DistanceArray[0]);
                Serial.print(", ");
                Serial.print(param->s_DistanceArray[1]);
                Serial.print(", ");
                Serial.print(param->s_DistanceArray[2]);
                Serial.print("]");
            }
            unsigned long d_array[3];

            d_array[0] = param->s_DistanceArray[0];
            d_array[1] = param->s_DistanceArray[1];
            d_array[2] = param->s_DistanceArray[2];

            param->s_MotorOutputs[0] = 255;
            param->s_MotorOutputs[1] = 255;

            // left sensor
            if (d_array[0] < param->sensor_threshold)
            {
                param->s_MotorOutputs[1] = 0; // enable opposite motor
            }

            // right sensor
            if (d_array[2] < param->sensor_threshold)
            {
                param->s_MotorOutputs[0] = 0; // enable opposite motor
            }

            // middle sensor
            if (d_array[1] < param->sensor_threshold)
            {
                param->s_MotorOutputs[0] = 0;
                param->s_MotorOutputs[1] = 0;
            }

            // if any sensor is within the theshold; engage evasive maneuvers, aka reverse
            /*if(
            d_array[0] < param->sensor_threshold ||
            d_array[1] < param->sensor_threshold ||
            d_array[2] < param->sensor_threshold)
            {
                param->s_MotorOutputs[0] = 0;
                param->s_MotorOutputs[1] = 0;

            }
            else // otherwise go forward. motor speed is based on side sensor's value
            {
                //map [0-350] to [0-255]
                //cap
                //param->s_MotorOutputs[0] = map(d_array[0], 0, 350, 0, 255);
                //param->s_MotorOutputs[1] = map(d_array[2], 0, 350, 0, 255);
                param->s_MotorOutputs[0] = 255;
                param->s_MotorOutputs[1] = 255;
            }*/
        }
        else
        {
            if (TASKDEBUG)
                Serial.print("Skipping Control\n");
        }
    }
}

struct Unified_Task *InitControlTask(
    bool *s_SensorReady, SemaphoreHandle_t m_SensorReady,
    unsigned long *s_DistanceArray, SemaphoreHandle_t m_DistanceArray,
    uint32_t *s_MotorOutputs, SemaphoreHandle_t m_MotorOutputs,
    char *s_DisplayContent, SemaphoreHandle_t m_DisplayContent)
{
    struct Unified_Task *uTask = (struct Unified_Task *)malloc(sizeof *uTask);
    struct Task_Information *info = (struct Task_Information *)malloc(sizeof *info);
    struct Task_Constraints *cnst = (struct Task_Constraints *)malloc(sizeof *cnst);
    struct Param *param = (struct Param *)malloc(sizeof *param);

    // init shared variables here
    param->s_SensorReady = s_SensorReady;
    param->m_SensorReady = m_SensorReady;
    param->s_DistanceArray = s_DistanceArray;
    param->m_DistanceArray = m_DistanceArray;
    param->s_MotorOutputs = s_MotorOutputs;
    param->m_MotorOutputs = m_MotorOutputs;
    param->s_DisplayContent = s_DisplayContent;
    param->m_DisplayContent = m_DisplayContent;

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