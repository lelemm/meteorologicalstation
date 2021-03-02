#include <Arduino.h>
#include "station.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "LeandroPatricia"
#define WIFI_PASSWORD "lp010912"

// Raspberry Pi Mosquitto MQTT Broker
#define MQTT_HOST "192.168.1.30"
// For a cloud MQTT broker, type the domain name
//#define MQTT_HOST "example.com"
#define MQTT_PORT 1883

WiFiClient espClient;
PubSubClient client(espClient);

Station* station;

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("EstacaoMetLightning", "lelemm", "Plllf00!")) {
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
  Serial.begin(115200);

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

bool lightning;
int lightningDistance;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  static char     buf[16];                        // sprintf text buffer

  if(now - lastRead > 100 || lastRead == 0) {
    station->getData(lightning, lightningDistance);  
    lastRead = now;
  }

  if (now - lastMsg > 60000 || lastMsg == 0 || lightning) {
    lastMsg = now;

    if(loopCount % 10 == 0)
    {
      Serial.println(F("Raio\tDist"));
    }

    loopCount++;

    DynamicJsonDocument doc(1024);

    doc["lightning"] = lightning;
    doc["distance"] = lightningDistance;

    String output;
    serializeJson(doc, output);

    client.publish("outdoor/lightning", output.c_str());

    char chars[16];
    Serial.print(lightning == true ? "Y\t" : "N\t");

    sprintf(buf, "%2d\n",
            lightningDistance);   // Temp in decidegrees
    Serial.print(buf);
  }
}