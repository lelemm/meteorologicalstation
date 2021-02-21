#include <Arduino.h>
#include "lightning_sensor.h"

LightningSensor::LightningSensor(int _interruptPin)
{
    lightning = new SparkFun_AS3935(AS3935_ADDR);
    lightInt = _interruptPin;

    pinMode(lightInt, INPUT); 

    if( !lightning->begin() ) { // Initialize the sensor. 
        throw;
    }

    lightning->setIndoorOutdoor(OUTDOOR); 
}

bool LightningSensor::hasData()
{
    distance = -1;

    if(digitalRead(lightInt) == HIGH)
    {
        // Hardware has alerted us to an event, now we read the interrupt register
        // to see exactly what it is. 
        int intVal = lightning->readInterruptReg();
        if(intVal == NOISE_INT)
        {
            return false;
        }
        else if(intVal == DISTURBER_INT)
        {
            return false;
        }
        else if(intVal == LIGHTNING_INT)
        {
            distance = lightning->distanceToStorm(); 
            return true;
        }
    }

    return false;
}

int LightningSensor::getDistance()
{
    return distance;
}