# GardenControl

Automatic irrigation with a cistern pump controlled by Wemos D1 Mini Arduino microcontroller via MQTT

## REQUIREMENTS

- Wemos D1 Mini (should work with other Arduino microcontrollers too)
- PlatformIO or Arduino IDE (for Arduino IDE you have to move files from /src to root-folder)
- MQTT Broker such as the Mosquitto MQTT Broker (https://mosquitto.org/)
- ControllApp such as MQTT Dash (https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en)

## INSTALL AND USAGE

### PINOUT

| Sensor-Description | Sensor-PIN | Arduino-PIN |
| --- | --- | --- | --- | 
| JSN-SR04T-2.0 Ultrasonic Sensor	| VCC	| 5.0 V	|
| JSN-SR04T-2.0 Ultrasonic Sensor	| Trigger |	D5 |
| JSN-SR04T-2.0 Ultrasonic Sensor	| ECHO	| D6 |
| JSN-SR04T-2.0 Ultrasonic Sensor	| GND | - |
| SRD-05VDC-SL-C Relais	| GND | - |
| SRD-05VDC-SL-C Relais	| IN1	| D1 |
| SRD-05VDC-SL-C Relais	| IN2	| D2 |
| SRD-05VDC-SL-C Relais	| VCC	| 5.0 V |	
| Button | VCC | 5.0 V |
| Button | GND | GND |
| Button | DAT | D4 |
| Feuchtigkeitssensor | VCC	| 5.0 V	|
| Feuchtigkeitssensor	| GND	| GND	|
| Feuchtigkeitssensor	| A0	| A0	|
| SRD-05VDC-SL-C Relais	| K1 | Phase L Pump |
| SRD-05VDC-SL-C Relais	| K2 | Phase L Socket |

### LIBRARIES ###

- PubSubClient by Nick O'Leary
- esp8266_mdns by dunk

## FEATURES

## DIRECTORIES AND FILES

- src/GardenControl.ino - The main programm code
- src/config.h - GPIO configuration
- src/secrets.h.sample - Sample configuration file for WiFi and MQTT credentials (please rename to secrets.h)
- README.md - The manual for this Arduino Script
- LICENSE - The license notes for this Arduino script
- platformio.ini - Wemos D1 Mini Configuration for PlatformIO

## HISTORY

## LICENSE

GardenControl and all individual scripts are under the BSD 3-Clause license unless explicitly noted otherwise. Please refer to the LICENSE
