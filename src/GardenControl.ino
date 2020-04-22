#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include "secrets.h"
#include "config.h"

const char *ssid = WIFI_SSID;
const char *password =  WIFI_PASSWORD;
const char *mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT;
const char *mqttUser = MQTT_USERNAME;
const char *mqttPassword = MQTT_PASSWORD;
const char *mqttID = MQTT_ID;
const int relayPump = RELAY_PUMP;
const int relaySocket = RELAY_SOCKET;

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  pinMode(LED_BUILTIN, OUTPUT);
  reconnect();
  
  client.subscribe("home/outdoor/cistern/socket");
  client.subscribe("home/outdoor/cistern/pump");
}

void reconnect() {

  while (!client.connected()) {

    WiFi.mode(WIFI_STA);
    delay(100);
    Serial.println();
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
 
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected to WiFi network");
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
  
    //IPAddress mqttServerIP = MDNS.queryHost(mqttServer);

    Serial.println("Connecting to MQTT broker");
    Serial.print("  MQTT Server: ");
    Serial.println(mqttServer);
    //Serial.print("  MQTT Server IP: ");
    //Serial.println(mqttServerIP.toString());
    Serial.print("  MQTT Port: ");
    Serial.println(mqttPort);
    Serial.print("  MQTT Username: ");
    Serial.println(mqttUser);
    Serial.print("  MQTT Identifier: ");
    Serial.println(mqttID);

    while (!client.connected()) {
      if (client.connect(mqttID, mqttUser, mqttPassword)) {
       Serial.println("Connected to MQTT broker");
       Serial.println("");
       digitalWrite(LED_BUILTIN, HIGH); 
       } else {
        Serial.print("Connection to MQTT broker failed with state ");
        Serial.println(client.state());
        delay(4000);
       }
    }
  }
}

void mqttloop()
{
  if (!client.loop())
    client.connect("ESP8266Client");
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  /*
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------"); */

  setCisternStatus(topic, payload, length);
}
 
void loop() {
  client.loop();
  mqttloop();
  reconnect();
}

void setCisternStatus(char* topic, byte* payload, unsigned int length) {

  // If the pin isn’t connected to anything, digitalRead() can return either HIGH or LOW (and this can change randomly).
  #define RELAY_ON 1 // equivalent to HIGH
  #define RELAY_OFF 0 // equivalent to LOW

  String mqttTopic = String(topic);
  String mqttPayload;
  for (unsigned int i = 0; i < length; i++) {
    mqttPayload += (char)payload[i];
    }
  String pinStatus;

  if (mqttTopic == "home/outdoor/cistern/pump")
  {
    if (mqttPayload == "on") {
      Serial.println("Switch on cistern pump");
      digitalWrite(relayPump, RELAY_ON);
      pinStatus = digitalRead(relayPump);
      Serial.print("Status of GPIO pin ");
      Serial.print(relayPump);
      Serial.print(" is ");
      Serial.println(pinStatus);
    } else if (mqttPayload == "off") {
      Serial.println("Switch off cistern pump");
      digitalWrite(relayPump, RELAY_OFF);
      pinStatus = digitalRead(relayPump);
      Serial.print("Status of GPIO pin ");
      Serial.print(relayPump);
      Serial.print(" is ");
      Serial.println(pinStatus);
    } else {
      Serial.println("No valid mqtt command");
    }
  }
  Serial.println("");  
}