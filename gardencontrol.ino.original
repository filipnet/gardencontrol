#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "Custom_Ultrasonic.h" 
#include "Custom_Relais.h"
#include "Custom_Button.h"
#include "Custom_SoilMoisture.h"

// Type: dyp-me007y
                               /* Änderungen in NewPing.cpp:  +: NewPing::NewPing(){};
                               Änderungen in NewPing.h:  class NewPing {
                               public:
                               +: NewPing(); */

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;
int count = 0;

// Distance messuring
long measure_now_level = millis();
long measure_last_level = 0;

// Button state
String buttonstate = "off";

// Network Definitions
WiFiClientSecure espClient;
PubSubClient client(espClient);

// Define constants
const char *ssid = "WonderLANd_IoT";
const char *password = "DerTeufelmitden3goldenenHaaren";
const char *hostname = "esp8266-gardencontrol";
const char *mqtt_id = "gardencontrol";
const char *mqtt_server = "mqtt.filipnet.de";
const int mqtt_port = 8883;
const char *mqtt_user = "gardencontrol";
const char *mqtt_pass = "XwQTUgzYcVmdLEx5JvthCGpbM9eyofFa";
const char *ota_id = "esp8266-gardencontrol";

// Ultrasonic definitions
#define TRIGGER_PIN D7
#define ECHO_PIN D6
int height = 600; // Original 312
Ultrasonic distance(TRIGGER_PIN, ECHO_PIN, height);

// Relais pump definitions
#define RELAIS1_PIN D1
Relais relais_pump(RELAIS1_PIN);

// Relais socket  Definitions
#define RELAIS2_PIN D2
Relais relais_socket(RELAIS2_PIN);

// Button definitions
#define BUTTON_PIN D4
Button button(BUTTON_PIN);

// Soilmoisture definitions
#define SOILMOISTURE_PIN A0
SoilMoisture soilmoisture(SOILMOISTURE_PIN);

// Initial setup
void setup()
{
  Serial.begin(115200);
  Serial.println("Booting...");

  // Turn cistern pump off on startup
  relais_pump.setstatus("off");

  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("WiFi: connected");
  Serial.println("");
  Serial.print("Connecting to MQTT-Server: ");
  Serial.println(mqtt_server);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  /* http://forum.arduino.cc/index.php?topic=474759.0
      In my case i'm using a 3v ProMini with the JSN-SR04T-2.0 @ 5v. Rather than using an external 4.7k pull up resistor (Echo to 5v)
    I enabled the internal pullup resistor "pinMode(echoPIN,INPUT_PULLUP);" and that worked. */
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT_PULLUP);

  /* OTA Configuration Block */

  // ArduinoOTA.setPort(8266);						// Port defaults to 8266
  ArduinoOTA.setHostname(ota_id); // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setPassword("admin");				// No authentication by default
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3	// Password can be set with it's md5 value as well
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect function and mqtt subscribe
void reconnect()
{
  if (!client.connected())
  {
    while (!client.connected())
    {
      Serial.print("MQTT Server: ");

      if (client.connect(mqtt_id, mqtt_user, mqtt_pass))
      {
        // State: Connected to MQTT
        Serial.println("connected");
        client.subscribe("home/outdoor/cistern/pump");
        client.subscribe("home/outdoor/cistern/socket");
      }
      else
      {
        // State: Disconnected from MQTT, trying to reconnect

        // Turn off cistern pump for security reasons
        Serial.println("Turn off cistern pump for security reasons cause no MQTT server is reachable");
        relais_pump.setstatus("off");

        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}

void mqttloop()
{
  if (!client.loop())
    client.connect("ESP8266Client");
}

void mqttsend(const char *_topic, const char *_data)
{
  client.publish(_topic, _data);
}

// Loop function
void loop()
{
  reconnect();
  mqttloop();
  ArduinoOTA.handle(); // Handle arduino OTA connections

  // Button state
  buttonstate = button.readbutton();

  if (buttonstate.equals("on"))
  {
    if (count == 0)
    {
      count = 1;
      client.publish("home/outdoor/cistern/button", "on");
    }
  }
  else
  {
    if (count == 1)
    {
      count = 0;
      client.publish("home/outdoor/cistern/button", "off");
    }
  }

  // Distance messure
  measure_now_level = millis();
  if (measure_now_level - measure_last_level > 5000)
  {
    measure_last_level = now;

    //String cisternlevel = (String)distance.getlevel();
    String cisternlevel = (String)distance.getlevel();

    char _level[sizeof(cisternlevel)];
    cisternlevel.toCharArray(_level, sizeof(_level));

    mqttsend("home/outdoor/cistern/level/raw", _level);

    char _cisternheight[sizeof((String)distance.getheight())];
    ((String)distance.getheight()).toCharArray(_cisternheight, sizeof(_cisternheight));

    mqttsend("home/outdoor/cistern/height", _cisternheight);
  }

  // Soilmoisture messure
  now = millis();
  if (now - lastMeasure > 20000)
  {
    lastMeasure = now;

    //MQTTSEND
    char _soilmoisture[sizeof((String)soilmoisture.getSoilMoisture())];
    ((String)soilmoisture.getSoilMoisture()).toCharArray(_soilmoisture, sizeof(_soilmoisture));

    mqttsend("home/outdoor/cistern/soilmoisture/raw", _soilmoisture);
  }
}

void callback(String topic, byte *message, unsigned int length)
{

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(" | Message: ");
  String messageTemp;

  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Turn cistern pump on and off
  if (topic == "home/outdoor/cistern/pump")
  {

    if (messageTemp == "on")
    {
      relais_pump.setstatus("on");
      Serial.println("Turn on cistern pump");
    }
    if (messageTemp == "off")
    {
      relais_pump.setstatus("off");
      Serial.println("Turn off cistern pump");
    }
  }

  // Turn socket on and off
  if (topic == "home/outdoor/cistern/relay_socket")
  {

    if (messageTemp == "on")
    {
      relais_socket.setstatus("on");
      Serial.println("Turn on socket on cistern");
    }
    if (messageTemp == "off")
    {
      relais_socket.setstatus("off");
      Serial.println("Turn off socket on cistern");
    }
  }
}
