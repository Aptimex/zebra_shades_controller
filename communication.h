#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define SSP_RX_PIN -1 //not needed
#define SSP_TX_PIN 14 //D5
SoftwareSerial SSP(SSP_RX_PIN, SSP_TX_PIN);

void sendBurst(const int *data, int len);

void up1();
void upAll();
void upBtnPressed();
void upBtnReleased();
void upBtnLongPressed();
void upBtnLongReleased();


void down1();
void downAll();
void downBtnPressed();
void downBtnReleased();
void downBtnLongPressed();
void downBtnLongReleased();

void halt();

#endif
