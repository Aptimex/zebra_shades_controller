#ifndef UTIL_H
#define UTIL_H

bool payloadEq(byte *payload, unsigned int plen, char *str);
void printBytesAsChar(byte *b, unsigned int len);
void printBytesAsHex(byte *b, unsigned int len);
void LEDOn();
void LEDOff();

#endif
