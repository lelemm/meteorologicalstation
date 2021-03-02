#include <Arduino.h>
#include "station.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "LeandroPatricia"
#define WIFI_PASSWORD "lp010912"

// Raspberry Pi Mosquitto MQTT Broker
#define MQTT_HOST "192.168.1.30"
#define MQTT_PORT 1883

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 60        /* Time ESP32 will go to sleep (in seconds) */

WiFiClient espClient;
PubSubClient client(espClient);

Station* station;

long beginCode;
float voltagem;
float valorPin;

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("EstacaoMet", "lelemm", "Plllf00!")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
    }
  }
}

boolean wifiConnected = false;
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  beginCode = millis();
  Serial.begin(115200);
  
  delay(300);
  //analogReadResolution(11);
  //analogSetAttenuation(ADC_6db);

  pinMode(4, INPUT); 
  station = new Station();

  setup_wifi();
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
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
long lastMsg = 0;
long lastRead = 0;

float lux;
bool rain;
int temp, humidity, pressure, gas;

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.println("Voltagem: ");
  valorPin = float(analogRead(35));
  voltagem = (valorPin / 330.0f) * 4.17f;
  Serial.println(String(voltagem));
  Serial.println(String(valorPin));



  long now = millis();
  static char     buf[16];                        // sprintf text buffer

  if(now - lastRead > 100 || lastRead == 0) {
    station->getData(lux, rain, temp, humidity, pressure, gas);  
    lastRead = now;
  }

  if (now - lastMsg > 60000 || lastMsg == 0) {
    lastMsg = now;

    if(loopCount % 10 == 0)
    {
      Serial.println(F("\nLux\tRain\tTemp\xC2\xB0\x43\tHumid%\tPress\thPa\tAir m"));
    }

    loopCount++;

    DynamicJsonDocument doc(1024);

    doc["lux"] = lux;
    doc["rain"]  = rain;

    doc["temp"] = float(temp)/100.0f;
    doc["humidity"] = float(humidity)/1000.0f;
    doc["pressure"] = float(pressure)/100.0f;
    doc["gas"] = float(gas)/100.0f;
    doc["volts"] = voltagem;
    doc["treco"] = valorPin;

    String output;
    serializeJson(doc, output);
    Serial.println(output.c_str());

    client.publish("outdoor/main", output.c_str());
 
    char chars[16];
    sprintf(buf, "%s", ftoa(chars, lux, 0));   // Temp in decidegrees
    Serial.print(buf);
    Serial.print("\t");
    Serial.print(rain == true ? "Y\t" : "N\t");

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

    Serial.println("Time for sleep:");

  }

  if(voltagem < 3.4)
  {
    delay(3000);
    float sleepTime = (TIME_TO_SLEEP) - ((millis() - beginCode) / 1000);
    Serial.println(String(sleepTime));
    esp_sleep_enable_timer_wakeup(sleepTime * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
  }

}