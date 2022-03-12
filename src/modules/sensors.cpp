#include "modules/sensors.h"

#include <Arduino.h>

// humidity and temp sensor to calibrate Ultrasonic modules

// array of ultrasound sensor modules

// define sound speed in cm/uS
double SOUND_SPEED = 0.034;

struct USoundModule
{
    // trigger pin
    signed char trigPin;
    // echo pin
    signed char echoPin;
    long duration;
    float distance;
};

struct ScanParam
{
    // number of chained modules
    size_t count;
    // list of Ultrasonic modules
    struct USoundModule **ModuleList;
    // list of measured distances (cm)
    double *distanceList;
    // time between consecutive scans (us)
    double delay;
    // timeout in case of malfunction (us)
    unsigned long timeout = 1000000UL;
};

void SensorTask::func()
{
    int duration;
    size_t i;

    UltraSoundSensor uSModule = sensorList.front();

    std::list<UltraSoundSensor>::iterator uSensor;
    std::list<unsigned long>::iterator distance;
    uSensor = sensorList.begin();
    distance = distanceList.begin();

    for (uSensor = sensorList.begin(); uSensor != sensorList.end(); ++uSensor)
    while(1)
    {
        if()
        digitalWrite(uSensor->trigPin, LOW);
        delayMicroseconds(2);

        // set trigpin to high for 10 us
        digitalWrite(uSensor->trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(uSensor->trigPin, LOW);

        // Reads the echoPin, returns the sound wave travel time in microseconds
        duration = pulseIn(uSensor->echoPin, HIGH);

        // Calculate the distance
        *distance = duration * SOUND_SPEED / 2;
        ++distance;
    }
}

unsigned long UltraSoundSensor::Scan()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2); // delays are too small for vTaskDelay

    // set trigpin to high for 10 us
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    return pulseIn(echoPin, HIGH); // configure timeout?
}

void SensorTask::func(void *params)
{
    while (1)
    {
        UltraSoundSensor *ultraSoundSensor;
        for (int i = 0; i < nbSensors; i++)
        {
            distanceList[i] = ultraSoundSensor[i].Scan();
        }
    }
}