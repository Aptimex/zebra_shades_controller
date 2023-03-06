#ifndef CONFIG_H
#define CONFIG_H

//Don't forget to run certs-from-mozilla.py and upload to LittleFS! 

#define WIFI_SSID "WIFI network name"
#define WIFI_PASS "password123"
#define MDNS_NAME "shades" //also used as device hostname; .local is implied for DNS
#define TZ_LOCAL TZ_America_Denver //for SSL verification

#define MQTT_BROKER "mqtt.example.com"
#define MQTT_PORT 8883
#define MQTT_USER "mqtt_user"
#define MQTT_PASS "password123"
#define MQTT_TOPIC "home/shades"
#define MQTT_CLIENT_PREFIX "shades"

#endif
