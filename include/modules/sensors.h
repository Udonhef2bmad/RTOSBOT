#include"Unified_Task.h"

#include <iostream>
#include <iterator>
#include <list>
using namespace std;


/// @brief Ultra sound sensor module
class UltraSoundSensor
{
public:
    signed char trigPin;
    signed char echoPin;

    UltraSoundSensor();

    // use module to scan distance in meters
    unsigned long Scan();
};


/// @brief Sensor Task chains sensor scans from different sensors
class SensorTask: public Unified_Task
{
public:   
    list<UltraSoundSensor> sensorList;
    list<unsigned long> distanceList;
    int nbSensors;

    SensorTask();
    void uLoop();
};
