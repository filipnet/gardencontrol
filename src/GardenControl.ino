#include <Arduino.h>
#include <ESP8266WiFi.h>
#define MQTT_MAX_PACKET_SIZE 256
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include "credentials.h"
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

unsigned long heartbeat_previousMillis = 0;
const long heartbeat_interval = HEARTBEAT_INTERVALL;

unsigned long emergencystop_previousMillis = 0;
const long emergencystop_threshold = EMERGENCYSTOP_THRESHOLD;
bool emergencystop_running = false;

WiFiClientSecure espClient;
PubSubClient client(espClient);
 
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(relayPump, HIGH);
  pinMode(relayPump, OUTPUT);
  digitalWrite(relaySocket, HIGH);
  pinMode(relaySocket, OUTPUT);
  espClient.setInsecure();
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
    // https://pubsubclient.knolleary.net/api.html
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
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
    Serial.println("");

    while (!client.connected()) {
      if (client.connect(mqttID, mqttUser, mqttPassword)) {
       Serial.println("Connected to MQTT broker");
       Serial.println("");
       digitalWrite(LED_BUILTIN, HIGH); 
       } else {
        Serial.print("Connection to MQTT broker failed with state: ");
        Serial.println(client.state());
        char puffer[100];
        espClient.getLastSSLError(puffer,sizeof(puffer));
        Serial.print("TLS connection failed with state: ");
        Serial.println(puffer);
        Serial.println("");
        digitalWrite(relayPump, HIGH);
        delay(4000);
       }
    }
  }
}

// Function to receive MQTT messages
void mqttloop()
{
  if (!client.loop())
    client.connect("ESP8266Client");
}

// Function to send MQTT messages
void mqttsend(const char *_topic, const char *_data)
{
  client.publish(_topic, _data);
}

// Pointer to a message callback function called when a message arrives for a subscription created by this client.
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message topic: ");
  Serial.print(topic);
  Serial.print(" | Message Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  setCisternStatus(topic, payload, length);
}

void loop() {
  client.loop();
  mqttloop();
  reconnect();
  heartbeat();
  switchontime();
  emergencystop();
}

bool emergencystop_running_prev = false;
unsigned long emergencystop_switchon = 0;

void switchontime() {
  if (!emergencystop_running_prev && emergencystop_running) {
    emergencystop_switchon = millis();
    emergencystop_running_prev = emergencystop_running;
  }
}

unsigned long timeDeltaOld = 0;
void emergencystop() {
  String pinStatus;
  if (emergencystop_running) {
    unsigned long emergencystop_currentMillis = millis();
    
    unsigned long timeDelta = (emergencystop_currentMillis-emergencystop_switchon)/1000;
    if (timeDeltaOld != timeDelta) {
      Serial.print(timeDelta);
      Serial.print("..");
      timeDeltaOld = timeDelta;
    }
    
    if (emergencystop_currentMillis - emergencystop_switchon >= emergencystop_threshold) {
      Serial.println("Emergency STOP cistern pump");
      digitalWrite(relayPump, HIGH);
      pinStatus = digitalRead(relayPump);
      Serial.print("Status of GPIO pin ");
      Serial.print(relayPump);
      Serial.print(" is ");
      Serial.println(pinStatus);
      delay(1000);
      Serial.println("Send Emergency STOP signal to MQTT broker");
      Serial.println("");
      client.publish("home/outdoor/cistern/emergencystop", "on");
      client.publish("home/outdoor/cistern/pump", "off");
      emergencystop_running = false;
      emergencystop_running_prev = false;
    }
  }
}

void heartbeat() {
  unsigned long heartbeat_currentMillis = millis();
  if (heartbeat_currentMillis - heartbeat_previousMillis >= heartbeat_interval) {
    heartbeat_previousMillis = heartbeat_currentMillis;
    Serial.println("Send heartbeat signal to MQTT broker");
    Serial.println("");
    client.publish("home/cistern/heartbeat", "on");
  }
}

void setCisternStatus(char* topic, byte* payload, unsigned int length) {
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
      digitalWrite(relayPump, LOW);
      pinStatus = digitalRead(relayPump);
      Serial.print("Status of GPIO pin ");
      Serial.print(relayPump);
      Serial.print(" is ");
      Serial.println(pinStatus);
      delay(1000);
      emergencystop_running = true;
      emergencystop_running_prev = false;
    } else if (mqttPayload == "off") {
      Serial.println("Switch off cistern pump");
      digitalWrite(relayPump, HIGH);
      pinStatus = digitalRead(relayPump);
      Serial.print("Status of GPIO pin ");
      Serial.print(relayPump);
      Serial.print(" is ");
      Serial.println(pinStatus);
      delay(1000);
      emergencystop_running = false;
      emergencystop_running_prev = false;
    } else {
      Serial.println("No valid mqtt command");
    }
  }

  else if (mqttTopic == "home/outdoor/cistern/socket")
  {
    if (mqttPayload == "on") {
      Serial.println("Switch on socket");
      digitalWrite(relaySocket, LOW);
      pinStatus = digitalRead(relaySocket);
      Serial.print("Status of GPIO pin ");
      Serial.print(relaySocket);
      Serial.print(" is ");
      Serial.println(pinStatus);
      delay(1000);
    } else if (mqttPayload == "off") {
      Serial.println("Switch off socket");
      digitalWrite(relaySocket, HIGH);
      pinStatus = digitalRead(relaySocket);
      Serial.print("Status of GPIO pin ");
      Serial.print(relaySocket);
      Serial.print(" is ");
      Serial.println(pinStatus);
      delay(1000);
    } else {
      Serial.println("No valid mqtt command");
    }
  }
  Serial.println("");  
}