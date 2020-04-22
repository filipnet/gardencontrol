#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include "config.h"

const char *ssid = WIFI_SSID;
const char *password =  WIFI_PASSWORD;
const char *mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT;
const char *mqttUser = MQTT_USERNAME;
const char *mqttPassword = MQTT_PASSWORD;
const char *mqttID = MQTT_ID;

WiFiClientSecure espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);
  reconnect();
  client.publish("esp/test", "Hello from ESP8266");
  client.subscribe("esp/test");
 
}

void reconnect() {
  // Loop until we're reconnected
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
       } else {
        Serial.print("Connection to MQTT broker failed with state ");
        Serial.println(client.state());
        delay(4000);
       }
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}
 
void loop() {
  client.loop();
  reconnect();
}