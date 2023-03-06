#include "util.h"

bool payloadEq(byte *payload, unsigned int plen, char *str) {
  //char arrays are always null-term'ed
  int slen = strlen(str); //strlen doesn't count \0

  dp("checking ");
  printBytesAsChar(payload, plen);
  dp( "[");
  dp(plen);
  dp("] against ");
  dp(str);
  dp( "[");
  dp(slen);
  dpln("]");
  
  if (plen != slen) {
    dpln("size mismatch");
    return false;
  }

  for (int i = 0; i < plen; i++) {
    if ((char)payload[i] != str[i]) {
      dp("char mismatch at pos ");
      dpln(i);
      return false;
    }
  }
  return true;
}

void printBytesAsChar(byte *b, unsigned int len) {
  for (unsigned int i = 0; i < len; i++) {
    dp((char)b[i]);
  }
}

void printBytesAsHex(byte *b, unsigned int len) {
  for (unsigned int i = 0; i < len; i++) {
    dphex(b[i]);
  }
}

void LEDOn() {
  digitalWrite(LED_BUILTIN, LOW); //low is on
  dpln("ON");
}

void LEDOff() {
  digitalWrite(LED_BUILTIN, HIGH); //high is off
  dpln("OFF");
}
