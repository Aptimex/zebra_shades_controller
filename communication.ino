//Values for shades communication (UART)
const int burst = 3;
const int burstDelay = 14; //ms
const int start = 0xff;
const int initial = 0x55;

void sendBurst(const int *data, int len) {
  if (len < 1) {
    return;
  }

  for (int b = 0; b < burst; b++) {
    for (int i = 0; i < len; i++) {
      SSP.write(data[i]);
      dphex(data[i]);
    }
    
    delay(burstDelay);
  }
  LEDOff();
  delay(100);
  LEDOn();
}

//Simulates pressing and releasing the button
void up1() {
  const int upJog1[3] = {initial, 0x11, 0x44};
  const int upJog2[3] = {initial, 0x15, 0x40};

  SSP.write(start);
  dphex(start);
  
  sendBurst(upJog1, 3);
  sendBurst(upJog2, 3);
}

//Simulates pressing and holding the button for 2s
void upAll() {
  const int upJog1[3] = {initial, 0x11, 0x44};
  const int upALL1[3] = {initial, 0x12, 0x47};
  const int upALL2[3] = {initial, 0x16, 0x43};

  SSP.write(start);
  dphex(start);
  
  //sendBurst(upJog1, 3);
  //delay(2000);
  sendBurst(upALL1, 3);
  sendBurst(upALL2, 3);
}


void upBtnPressed() {
  const int upJog1[3] = {initial, 0x11, 0x44};

  SSP.write(start);
  dphex(start);
  
  sendBurst(upJog1, 3);
}

void upBtnReleased() {
  const int upJog2[3] = {initial, 0x15, 0x40};
  sendBurst(upJog2, 3);
}

void upBtnLongPressed() {
  const int upALL1[3] = {initial, 0x12, 0x47};

  SSP.write(start);
  dphex(start);
  
  sendBurst(upALL1, 3);
}

void upBtnLongReleased() {
  const int upALL2[3] = {initial, 0x16, 0x43};
  sendBurst(upALL2, 3);
}


//Simulates pressing and releasing the button
void down1() {
  const int downJog1[3] = {initial, 0x31, 0x64};
  const int downJog2[3] = {initial, 0x35, 0x60};

  SSP.write(start);
  dphex(start);
  
  sendBurst(downJog1, 3);
  sendBurst(downJog2, 3);
}

//Simulates pressing and holding the button for 2s
void downAll() {
  const int downJog1[3] = {initial, 0x31, 0x64};
  const int downALL1[3] = {initial, 0x32, 0x67};
  const int downALL2[3] = {initial, 0x36, 0x63};

  SSP.write(start);
  dphex(start);
  
  //sendBurst(downJog1, 3);
  //delay(2000);
  sendBurst(downALL1, 3);
  sendBurst(downALL2, 3);
}

void downBtnPressed() {
  const int downJog1[3] = {initial, 0x31, 0x64};

  SSP.write(start);
  dphex(start);
  
  sendBurst(downJog1, 3);
}

void downBtnReleased() {
  const int downJog2[3] = {initial, 0x35, 0x60};
  sendBurst(downJog2, 3);
}

void downBtnLongPressed() {
  const int downALL1[3] = {initial, 0x32, 0x67};

  SSP.write(start);
  dphex(start);
  
  sendBurst(downALL1, 3);
}

void downBtnLongReleased() {
  const int downALL2[3] = {initial, 0x36, 0x63};
  sendBurst(downALL2, 3);
}



void halt() {
  const int stop1[3] = {initial, 0x21, 0x74};
  const int stop2[3] = {initial, 0x25, 0x70};

  SSP.write(start);
  dphex(start);
  
  sendBurst(stop1, 3);
  sendBurst(stop2, 3);
}
