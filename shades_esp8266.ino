#include <PubSubClient.h>
//#include <ESP8266WiFi.h> //doesn't support SSL
#include <WiFiClientSecureBearSSL.h> //https://www.dzombak.com/blog/2021/10/HTTPS-Requests-with-a-Root-Certificate-Store-on-ESP8266-Arduino.html
#include <ESP8266mDNS.h>
#include <time.h>
#include <LittleFS.h>
#include <TZ.h>

//Avoid conflict with USB Serial for debugging, and a conflict with the builtin LED on GPIO2/D4 (when using Serial1)
//For the low baudrate and short, infrequent UART communication used here, the limitations of software serial are a non-issue
//Install the ESPSoftwarSserial library
#include <SoftwareSerial.h> 

//#include "shades_esp8266.h"
#include "buttons.h"
#include "communication.h"
#include "config.h"
#include "mqtt.h"
#include "util.h"



//uncomment this line to enable debug printing over USB
#define DEBUG 1

#ifdef DEBUG
  #define USB_WAIT_READY(x) while(!Serial) {;}
  #define dp(x)  Serial.print (x)
  #define dphex(x)  Serial.print (x, HEX)
  #define dpln(x)  Serial.println (x)
#else
  #define USB_WAIT_READY(x) 
  #define dp(x)
  #define dphex(x)
  #define dpln(x)
#endif



//config.h
const char ssid[] = WIFI_SSID;
const char pass[] = WIFI_PASS;
const char mdns_name[] = MDNS_NAME;

//Connection objects
//WiFiSSLClient wifiClient;
BearSSL::WiFiClientSecure wifiClient;
BearSSL::CertStore certStore;
PubSubClient PSClient(broker, port, subCallback, wifiClient); //mqtt.h

//buttons.h
extern const int upBtn;
extern const int haltBtn;
extern const int downBtn;

//communication.h
extern SoftwareSerial SSP;

//int wifiStatus = WL_IDLE_STATUS;
long lastMQTTReconnectAttempt = -10000;



void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // D4, GPIO2
  LEDOff();
  buttonSetup();
  timerSetup();

  #ifdef DEBUG
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  #endif
  //USB_WAIT_READY();

  //Generate the certs file with certs-from-mozilla.py
  //Write the certs to the chip with Tools->ESP8266 LittleFS Data Upload
  LittleFS.begin();
  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  dp("Read ");
  dp(numCerts);
  dpln(" certs into store");
  if (numCerts == 0) {
      dpln(F("!!! No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory?"));
      return;
  }
  wifiClient.setCertStore(&certStore);

  wifiStart();

  /*
  //Initialize serial for shade control
  //If debug mode is on, send the shade UART commands over Serial1 (unconnected pin) to avoid a conflict with the USB Serial Monitor
  #ifdef DEBUG
    //Note that Serial1 uses GPIO2 (D4) for TX, which is the same as the BUILTIN_LED, so the LED will be tied to TX when debug is inabled. 
    //Serial1 does not have an RX pin. 
    //https://arduino-esp8266.readthedocs.io/en/latest/reference.html#serial
    Serial1.begin(9600);
  #else
    //For non-debug runs, use the normal serial TX pin (which is shared with USB serial) so the LED can be controlled as expected
    Serial.begin(9600);
  #endif
  */
  SSP.begin(9600);
  if (!SSP) { // If the object did not initialize, then its configuration is invalid
    dpln("Invalid SoftwareSerial pin configuration, check config"); 
    while (1) { // Don't continue with invalid configuration
      LEDOff();
      delay (1000);
      LEDOn();
      delay (1000);
    }
  }
}

void wifiStart() {
  delay(10);
  dp("Connecting to ");
  dpln(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.hostname(mdns_name);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    dp(".");
  }

  //randomSeed(micros());

  dpln("");
  dp("WiFi connected: ");
  dpln(WiFi.localIP());

  if (MDNS.begin(mdns_name)) {
    dp("mDNS responder started: ");
    dpln(mdns_name);
  } else {
    dpln("Error setting up MDNS responder!");
  }

  setClock();
}

// Set time via NTP, as required for x.509 validation
void setClock() {
  //configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
   configTime(TZ_LOCAL, "pool.ntp.org", "time.nist.gov");

  dp("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    dp(".");
    now = time(nullptr);
  }
  dpln("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  dp("Current time: ");
  dpln(asctime(&timeinfo));
}

void loop() {
  checkButtons();
  
  //Handle wifi disconnects
  static long lastWifiReconnectAttempt = 0;
  int status = WiFi.status();
  //if (status==WL_DISCONNECTED || status==WL_CONNECTION_LOST) {
  if (status != WL_CONNECTED) {
    
    if (millis() - lastWifiReconnectAttempt > 10000) {
      LEDOff();
      dpln("Wifi reconnect time");
      dp("wifi status: ");
      dpln(WiFi.status());
      dp("wifiClient connected: ");
      if (wifiClient.connected()) dpln("yes"); else dpln("no");
      //dp("WiFi Reason Code: ");
      //dpln(WiFi.reasonCode());

      LEDOn();
      delay(100);
      LEDOff();
      delay(100);
      
      status = WiFi.begin(ssid, pass);
      
      dp("New wifi status: ");
      dpln(WiFi.status());
      dp("wifiClient connected: ");
      if (wifiClient.connected()) dpln("yes"); else dpln("no");
      //dp("WiFi Reason Code: ");
      //dpln(WiFi.reasonCode());
      
      lastWifiReconnectAttempt = millis();

      if (status == WL_CONNECTED) {
        if (MDNS.begin(mdns_name)) {
          dpln("mDNS responder started");
        } else {
          dpln("Error setting up MDNS responder!");
        }

        setClock();
        
        LEDOn();
        delay(1000);
        LEDOff();
        delay(500);
      }
    }
  }
  

  
  //If disconnected, attempt to reconnect to MQTT broker every 10s
  if (WiFi.status() == WL_CONNECTED && !PSClient.connected()) {
    //dp("MQTT disconnected: ");
    //dpln(PSClient.state());
    
    if ((millis() - lastMQTTReconnectAttempt) > 10000) {
      LEDOff();
      dpln("MQTT reconnect time!");

      mqttReconnect();
      
      dp("wifi status: ");
      dpln(WiFi.status());
      dp("wifiClient connected: ");
      if (wifiClient.connected()) dpln("yes"); else dpln("no");
      //dp("WiFi Reason Code: ");
      //dpln(WiFi.reasonCode());
      dpln();
      
      lastMQTTReconnectAttempt = millis();
    }
  } else { //Connected
    PSClient.loop();
  }
}
