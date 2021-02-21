#include "rain_detector.h"
#include <BH1750_WE.h>
#include "lightning_sensor.h"
#include "air_sensor.h"

class Station
{
private:
    RainDetector* rainDetector;
    BH1750_WE* lightSensor;
    LightningSensor* lightningSensor;
    AirSensor* airSensor;
public:
    Station();
    void getData(float &lux, bool &rain, bool &lightning, int &lightningDistance, int &temp, int &humidity, int &preassure, int &gas);
};
