#include "station.h"

Station::Station(/* args */)
{
  Wire.begin();
  //rainDetector = new RainDetector(34);
  lightSensor = new BH1750_WE(0x23);
  lightSensor->init();
  airSensor = new AirSensor();
}

void Station::getData(float &lux, bool &rain, int &temp, int &humidity, int &pressure, int &gas)
{
    lux = lightSensor->getLux();
//    rain = rainDetector->isRainning();

    airSensor->getSensorData(temp, humidity, pressure, gas);

}
