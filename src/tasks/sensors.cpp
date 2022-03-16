#include "tasks/sensors.h"

namespace // hidden
{
    bool TASKDEBUG = true;

    // pin definition for HC-SR04 ultrasound sensor
    struct Distance_Sensor
    {
        uint8_t trigPin;
        uint8_t echoPin;
        unsigned long timeout;
        // long duration;
        // float distance;
    };

    // perform an ultrasound measurement
    unsigned long Measure(struct Distance_Sensor *sensor)
    {
        digitalWrite(sensor->trigPin, LOW);
        delayMicroseconds(2); // delays are too small for vTaskDelay
        // set trigpin to high for 10 us
        digitalWrite(sensor->trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(sensor->trigPin, LOW);
        // Reads the echoPin, returns the sound wave travel time in microseconds
        //return pulseIn(sensor->echoPin, HIGH, sensor->timeout); // configure timeout?
        return pulseIn(sensor->echoPin, HIGH, 5000) ; // configure timeout?
    }

    struct Param
    {
        int sensor_count;                      // number of sensors
        struct Distance_Sensor **sensor_array; // array of sensors
        double sound_speed;                    // sound of speed in the air (unit?)
        double interval;                       // interval between each scan (ms)
        unsigned long timeout;                 // timeout applied to each sensor

        unsigned long *s_DistanceArray;    // shared array holding measured distances
        SemaphoreHandle_t m_DistanceArray; // mutex for s_DistanceArray
        
        bool *s_SensorReady;             // shared variable that signals compute task to run
        SemaphoreHandle_t m_SensorReady; // mutex for s_SensorReady

        char *s_DisplayContent;             // array that holds logging info to be displayed
        SemaphoreHandle_t m_DisplayContent; // mutex for s_SensorReady
    };

    void getParameters(struct Param *param)
    {
        // set parameters here

        // use 3 sensors
        param->sensor_count = 3;
        param->sensor_array =
            (struct Distance_Sensor **)malloc(param->sensor_count * (sizeof *(param->sensor_array)));
        // shared variable
        // param->s_DistanceArray =
        //     (double *)malloc(param->sensor_count * (sizeof *(param->s_DistanceArray)));
        struct Distance_Sensor *dSensor;

        param->timeout = 1000000UL; // set sensor timeout [WARNING : crashes when changed???]

        // first sensor
        dSensor = (struct Distance_Sensor *)malloc(sizeof *dSensor);
        dSensor->trigPin = 19;
        dSensor->echoPin = 18;
        dSensor->timeout = param->timeout;
        param->sensor_array[0] = dSensor;
        // second sensor
        dSensor = (struct Distance_Sensor *)malloc(sizeof *dSensor);
        dSensor->trigPin = 5;
        dSensor->echoPin = 17;
        dSensor->timeout = param->timeout;
        param->sensor_array[1] = dSensor;
        // third sensor
        dSensor = (struct Distance_Sensor *)malloc(sizeof *dSensor);
        dSensor->trigPin = 16;
        dSensor->echoPin = 4;
        dSensor->timeout = param->timeout;
        param->sensor_array[2] = dSensor;

        // misc config
        param->sound_speed = 0.034;// cm/us
        param->interval = 0.500;
    }

    void getConstraints(struct Task_Constraints *cnst)
    {
        // set constraints here
        cnst->xFirst = 0       / portTICK_PERIOD_MS; /// [O] First activation
        cnst->xWcet = 30         / portTICK_PERIOD_MS; /// [C] Worst-case execution time //unused?//
        cnst->xPeriod = 200      / portTICK_PERIOD_MS; /// [T] Period
        cnst->xDeadline = 5000    / portTICK_PERIOD_MS; /// [D] Deadline
    }

    void getInformation(struct Task_Information *info)
    {
        // set variables here
        info->name = "SensorTask";
        info->usStackDepth = 3000;
        info->pvParameters = (void *)1;
        info->uxPriority = 3;
        info->pvCreatedTask = NULL;
    }

    void uSetup(void *pvParameters)
    {
        struct Param *param = (struct Param *)pvParameters;
        struct Distance_Sensor **sensor_array = param->sensor_array;

        for (int i = 0; i < param->sensor_count; i++)
        {
            pinMode(sensor_array[i]->trigPin, OUTPUT); // Sets the trigPin as an Output
            pinMode(sensor_array[i]->echoPin, INPUT);
        }
    }

    void uLoop(void *pvParameters)
    {
        struct Param *param = (struct Param *)pvParameters;
        struct Distance_Sensor **sensor_array = param->sensor_array;

        for (int i = 0; i < param->sensor_count; i++)
        {
            param->s_DistanceArray[i] = Measure(sensor_array[i])* param->sound_speed / 2;
            if(param->s_DistanceArray[i] == 0)
            {
                param->s_DistanceArray[i] = 100;
            }

            if (TASKDEBUG)
            {
                Serial.print("[");
                Serial.print(param->s_DistanceArray[i]);
                Serial.print("]");
            }
        }
        if (TASKDEBUG)
        Serial.print("\n");
        // signal to control that reading are ready
        //if (TASKDEBUG)
        //    Serial.print("\nsetting ready flag\n");
        *(param->s_SensorReady) = true;
        //if (TASKDEBUG)
        //   Serial.print("done setting ready flag\n");
    }
}

struct Unified_Task *InitSensorTask(
    bool *s_SensorReady, SemaphoreHandle_t m_SensorReady,
    unsigned long* s_DistanceArray, SemaphoreHandle_t m_DistanceArray,
    char *s_DisplayContent, SemaphoreHandle_t m_DisplayContent)
{
    struct Unified_Task *uTask = (struct Unified_Task *)malloc(sizeof *uTask);
    struct Task_Information *info = (struct Task_Information *)malloc(sizeof *info);
    struct Task_Constraints *cnst = (struct Task_Constraints *)malloc(sizeof *cnst);
    struct Param *param = (struct Param *)malloc(sizeof *param);

    // init shared variables here
    param->s_SensorReady = s_SensorReady;
    param->m_SensorReady = m_SensorReady;
    param-> s_DistanceArray = s_DistanceArray;
    param-> m_DistanceArray = m_DistanceArray;
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
