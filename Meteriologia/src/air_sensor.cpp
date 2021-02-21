#include <Arduino.h>
#include "air_sensor.h"

AirSensor::AirSensor()
{
    BME680 = new BME680_Class();
    while (!BME680->begin(I2C_STANDARD_MODE)) {  // Start BME680 using I2C, use first device found

        delay(5000);
    }  // of loop until device is located

    BME680->setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
    BME680->setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
    BME680->setOversampling(PressureSensor, Oversample16);     // Use enumerated type values
    BME680->setIIRFilter(IIR4);  // Use enumerated type values
    BME680->setGas(320, 150);  // 320�c for 150 milliseconds
}

void AirSensor::getSensorData(int &temp, int &humidity, int &pressure, int &gas)
{
    BME680->getSensorData(temp, humidity, pressure, gas);
}
