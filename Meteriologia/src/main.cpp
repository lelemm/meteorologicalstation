#include <Arduino.h>
#include "station.h"

Station* station;

void setup() {
  Serial.begin(115200);
  station = new Station();

}

char *ftoa(char *a, double f, int precision)
{
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
 
 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 *a++ = '.';
 long desimal = abs((long)((f - heiltal) * p[precision]));
 itoa(desimal, a, 10);
 return ret;
}

int loopCount = 0;
void loop() {
  float lux;
  bool rain;
  bool lightning;
  int lightningDistance;
  int temp, humidity, pressure, gas;
  static char     buf[16];                        // sprintf text buffer

  if(loopCount % 10 == 0)
  {
    Serial.println(F("\nLux\tRain\tRaio\tDist\tTemp\xC2\xB0\x43\tHumid%\tPress\thPa\tAir m"));
  }

  loopCount++;
  char chars[16];
  station->getData(lux, rain, lightning, lightningDistance, temp, humidity, pressure, gas);
    sprintf(buf, "%s", ftoa(chars, lux, 0));   // Temp in decidegrees
    Serial.print(buf);
    Serial.print("\t");
    Serial.print(rain == true ? "Y\t" : "N\t");
    Serial.print(lightning == true ? "Y\t" : "N\t");

    sprintf(buf, "%2d\t",
            lightningDistance);   // Temp in decidegrees
    Serial.print(buf);

    sprintf(buf, "%3d.%02d\t",
            (int8_t)(temp / 100), (uint8_t)(temp % 100));   // Temp in decidegrees
    Serial.print(buf);
    sprintf(buf, "%3d.%03d\t", (int8_t)(humidity / 1000),
            (uint16_t)(humidity % 1000));  // Humidity milli-pct
    Serial.print(buf);
    sprintf(buf, "%7d.%02d\t", (int16_t)(pressure / 100),
            (uint8_t)(pressure % 100));  // Pressure Pascals
    Serial.print(buf);
    sprintf(buf, "%4d.%02d\n", (int16_t)(gas / 100), (uint8_t)(gas % 100));  // Resistance milliohms
    Serial.print(buf);


  delay(500);
}