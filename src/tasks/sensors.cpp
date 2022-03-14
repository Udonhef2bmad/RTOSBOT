#include "tasks/sensors.h"

namespace //hidden
{

    // pin definition for HC-SR04 ultrasound sensor
    struct Distance_Sensor
    {
        uint8_t trigPin;
        uint8_t echoPin;
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
        return pulseIn(sensor->echoPin, HIGH); // configure timeout?
    }

    struct Param
    {
        int sensor_count;                      // number of sensors
        struct Distance_Sensor **sensor_array; // array of sensors
        double *distance_array;                // array holding measured distances
        double sound_speed;                    // sound of speed in the air (unit?)
        double interval;                       // interval between each scan (ms)
        int test;
    };



    void getParameters(struct Param *param)
    {
        //set parameters here

        // use 3 sensors
        param->sensor_count = 3;
        param->sensor_array =
            (struct Distance_Sensor **)malloc(param->sensor_count * (sizeof *(param->sensor_array)));
        param->distance_array =
            (double *)malloc(param->sensor_count * (sizeof *(param->distance_array)));
        struct Distance_Sensor *dSensor;

        // first sensor
        dSensor = (struct Distance_Sensor *)malloc(sizeof *dSensor);
        dSensor->trigPin = 19;
        dSensor->echoPin = 18;
        param->sensor_array[0] = dSensor;
        // second sensor
        dSensor = (struct Distance_Sensor *)malloc(sizeof *dSensor);
        dSensor->trigPin = 5;
        dSensor->echoPin = 17;
        param->sensor_array[1] = dSensor;
        // third sensor
        dSensor = (struct Distance_Sensor *)malloc(sizeof *dSensor);
        dSensor->trigPin = 16;
        dSensor->echoPin = 4;
        param->sensor_array[2] = dSensor;

        //misc config
        param->sound_speed = 0.034;
        param->interval = 0.500;
        param->test = 500;
    }

    void getConstraints(struct Task_Constraints *cnst)
    {
        // set constraints here
        cnst->xFirst = 0;                           /// [O] First activation
        cnst->xWcet = 3000;                         /// [C] Worst-case execution time //unused?//
        cnst->xPeriod = 300 / portTICK_PERIOD_MS;   /// [T] Period
        cnst->xDeadline = 4000 / portTICK_PERIOD_MS;/// [D] Deadline
    }

    void getInformation(struct Task_Information *info)
    {
        // set variables here
        info->name = "SensorTask";
        info->usStackDepth = 1000; 
        info->pvParameters = (void *)1;
        info->uxPriority = 1;
        info->pvCreatedTask = NULL;
    }

    void uSetup(void *pvParameters)
    {
        struct Param *param = (struct Param *)pvParameters;
        struct Distance_Sensor **sensor_array = param->sensor_array;

        for (int i = 0; i < param->sensor_count; i++)
        {
            Serial.print("trigPin:");
            Serial.print((int)sensor_array[i]->trigPin);
            Serial.print("\nechoPin:");
            Serial.println((int)sensor_array[i]->echoPin);
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
            Serial.print("trigPin:");
            Serial.println(sensor_array[i]->trigPin);
            Serial.print("echoPin:");
            Serial.println(sensor_array[i]->echoPin);
            param->distance_array[i] = 
                Measure(sensor_array[i]) * param->sound_speed / 2;
            Serial.print("distance :");
            Serial.println(param->distance_array[i]);
        }
    }
}

struct Unified_Task *InitSensorTask()
{
    struct Unified_Task *uTask      = (struct Unified_Task *)malloc(sizeof *uTask);
    struct Task_Information *info   = (struct Task_Information *)malloc(sizeof *info);
    struct Task_Constraints *cnst   = (struct Task_Constraints *)malloc(sizeof *cnst);
    struct Param *param             = (struct Param *)malloc(sizeof *param);

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
