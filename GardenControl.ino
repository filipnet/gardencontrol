#include "config_wifi.h"
#include "config_mqtt.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "YourNetworkName";
const char* password =  "YourNetworkPassword";
const char* mqttServer = "mqtt.YourDomain.com";
const int mqttPort = 8883;
const char* mqttUser = "YourMqttUser";
const char* mqttPassword = "YourMqttUserPassword";
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);
 
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
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("Connected to the MQTT broker");  
 
    } else {
 
      Serial.print("Connection to MQTT broker failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("esp/test", "Hello from ESP8266");
  client.subscribe("esp/test");
 
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
}