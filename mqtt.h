#ifndef MQTT_H
#define MQTT_H

#include "config.h"

const char broker[] = MQTT_BROKER;
const int port = MQTT_PORT;
const char mqtt_user[] = MQTT_USER;
const char mqtt_pass[] = MQTT_PASS;
const char topic[]  = MQTT_TOPIC;
const char clientID[] = MQTT_CLIENT_PREFIX;

void mqttConnect();
bool mqttSubscribe();
bool mqttReconnect();
//bool payloadEq(byte *payload, unsigned int plen, char *str);
void subCallback(char *topic, byte* payload, unsigned int len);

//void printBytesAsChar(byte *b, unsigned int len);
//void printBytesAsHex(byte *b, unsigned int len);

#endif
