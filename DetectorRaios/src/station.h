#include "lightning_sensor.h"

class Station
{
private:
    LightningSensor* lightningSensor;
public:
    Station();
    void getData(bool &lightning, int &lightningDistance);
};
