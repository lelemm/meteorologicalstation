#include "rain_detector.h"
#include <BH1750_WE.h>
#include "air_sensor.h"

class Station
{
private:
    RainDetector* rainDetector;
    BH1750_WE* lightSensor;
    AirSensor* airSensor;
public:
    Station();
    void getData(float &lux, bool &rain, int &temp, int &humidity, int &preassure, int &gas);
};
