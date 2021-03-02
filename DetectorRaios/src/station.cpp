#include "station.h"

Station::Station(/* args */)
{
  Wire.begin();
  lightningSensor = new LightningSensor(4);
}

void Station::getData(bool &lightning, int &lightningDistance)
{
    lightning = lightningSensor->hasData();
    lightningDistance = lightningSensor->getDistance();
}
