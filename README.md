# GardenControl

Automatic irrigation with a cistern pump controlled by Wemos D1 Mini Arduino microcontroller via MQTT

## REQUIREMENTS

- Wemos D1 Mini (should work with other Arduino microcontrollers too)
- PlatformIO or Arduino IDE (for Arduino IDE you have to move files from /src to root-folder)

## INSTALL AND USAGE

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
