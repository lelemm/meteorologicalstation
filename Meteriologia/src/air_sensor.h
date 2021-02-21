#include "Zanshin_BME680.h"

class AirSensor
{
private:
    BME680_Class* BME680;
public:
    AirSensor();
    void getSensorData(int &temp, int &humidity, int &pressure, int &gas);
};
