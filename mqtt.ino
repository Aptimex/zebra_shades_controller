#include <PubSubClient.h>
#include "config.h"
#include "mqtt.h"
#include "util.h"

//Block until connected; only use this for initial setup
void mqttConnect() {
  String cid = String(clientID) + String("_") + String(random(0xffff), HEX);
  dp("Connecting to MQTT broker: ");
  dp(broker);
  dp(" as ");
  dpln(cid.c_str());
  while (!PSClient.connect(cid.c_str(), MQTT_USER, MQTT_PASS)) {
    dp(".");
    delay(3000);
  }
  
  dpln("Connected!");
  LEDOn();
  delay(100);
  LEDOff();
  delay(100);
  LEDOn();
  delay(100);
  LEDOff();
  delay(100);

  mqttSubscribe();
}

//Subscribe to the relevant topic(s)
bool mqttSubscribe() {
  dp("Subscribing: ");
  dpln(topic);
  if (PSClient.subscribe(topic)) {
    dpln("Subscribed!");
    LEDOn();
    return true;
  } else {
    dpln("Sub failed");
    return false;
  }
}

//Attempt to reconnect once
bool mqttReconnect() {
  String cid = String(clientID) + String("_") + String(random(0xffff), HEX);
  
  dp("Reconnecting to MQTT broker: ");
  dpln(broker);
  LEDOn();
  delay(100);
  LEDOff();
  delay(100);
  LEDOn();
  delay(100);
  LEDOff();
  delay(100);
  
  if (PSClient.connect(clientID, MQTT_USER, MQTT_PASS)) {
    LEDOn();
    delay(100);
    LEDOff();
    delay(100);
    LEDOn();
    delay(100);
    LEDOff();
    delay(100);
      
    if (mqttSubscribe()) {
      return true;
    } else {
      dpln("MQTT connected, but couldn't subscribe");
    }
  }
  dp("MQTT reconnect failed with code ");
  dpln(PSClient.state());
  return false;
}

void subCallback(char *topic, byte *payload, unsigned int len) {
  char u1[] = "up1";
  char d1[] = "down1";
  char ua[] = "upAll";
  char da[] = "downAll";
  char h[] = "halt";

  dp("Received payload: ");
  printBytesAsChar(payload, len);
  dpln();
  
  if (payloadEq(payload, len, u1)) {
    up1();
  } else if (payloadEq(payload, len, d1)) {
    down1();
  } else if (payloadEq(payload, len, ua)) {
    upAll();
  } else if (payloadEq(payload, len, da)) {
    downAll();
  } else if (payloadEq(payload, len, h)) {
    halt();
  } else {
    dp("Unknown command: ");
    for (int i = 0; i < len; i++) {
      dp((char)payload[i]);
    }
    dpln();

    for (int i = 0; i < 5; i++) {
      LEDOff();
      delay(100);
      LEDOn();
    }
    return;
  }

  LEDOff();
  delay(100);
  LEDOn();
}
