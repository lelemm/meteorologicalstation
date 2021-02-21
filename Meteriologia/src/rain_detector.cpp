#include <Arduino.h>
#include "rain_detector.h"

RainDetector::RainDetector(int _rainPin)
{
    rainPin = _rainPin;
}

bool RainDetector::isRainning()
{
    int rainValue = analogRead(rainPin);
    if(rainValue < 3000)
        return true;

    return false;
}
