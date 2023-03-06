#include <Ticker.h>

//#include "SAMDTimerInterrupt.h"
//#include "SAMD_ISR_Timer.h"
#include "buttons.h"
#include "communication.h"

// Select a Timer Clock
#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default
#include <ESP8266TimerInterrupt.h> //https://github.com/khoih-prog/ESP8266TimerInterrupt

//Timers
volatile bool timerInt = false;
//SAMDTimer ITimer0(TIMER_TC3);
//SAMD_ISR_Timer ISR_Timer;
//Ticker timer1;
ESP8266Timer ITimer0;

void buttonSetup() {
  //Setup pins; pullup mode means they will sit HIGH, so button should close to ground (LOW)
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(haltBtn, INPUT_PULLUP);
  pinMode(downBtn, INPUT_PULLUP);

  //Pin inturrupts; CHANGE enables checking time pressed; use LOW for just presses
  attachInterrupt(digitalPinToInterrupt(upBtn), INT_UP, CHANGE);
  attachInterrupt(digitalPinToInterrupt(haltBtn), INT_HALT, FALLING);
  attachInterrupt(digitalPinToInterrupt(downBtn), INT_DOWN, CHANGE);
}

//inturrupt mode must be CHANGE
void ICACHE_RAM_ATTR INT_UP() {
  if (digitalRead(upBtn) == LOW) {
    upPressed = true;
  } else {
    upReleased = true;
    timerStop();
  }
  return;
}

//inturrupt mode must be FALLING
void ICACHE_RAM_ATTR INT_HALT() {
  goHalt = true;
  return;
}

//inturrupt mode must be CHANGE
void ICACHE_RAM_ATTR INT_DOWN() {
  if (digitalRead(downBtn) == LOW) {
    downPressed = true;
  } else {
    downReleased = true;
    timerStop();
  }
  return;
}


void timerSetup() {
  //Returns the timerNumber, or -1 on failure
  //setTimeout causes it to run once instead of forever

  // Interval in microseconds
  if (ITimer0.attachInterruptInterval(2000 * 1000, timerHandler)) {
    dpln("Starting  ITimer0 OK, millis() = " + String(millis()));
    ITimer0.stopTimer();
  } else {
    dpln("Can't set ITimer0. Select another freq. or timer");
  }
  

  /*
  timer1_isr_init();
  //timer1_attachInterrupt(onTimerISR);
  timer1_attachInterrupt(timerHandler);
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);
  timer1_write(TIMER0_INTERVAL_MS * TIMER_INTERVAL_2S);
  */

  //timer1.attach(2, timerHandler);
}

void timerStart() {
  //ISR_Timer.run();
  ITimer0.restartTimer();
  //timer1.once_ms(TIMER_INTERVAL_2S, timerHandler);
}

void timerStop() {
  ITimer0.stopTimer();
  //timer1.detach();
}

//void ICACHE_RAM_ATTR timerHandler() {
void ICACHE_RAM_ATTR timerHandler() {
  dpln("T");
  //upAll or downAll
  //Assumes the timer will be stopped by the button release inturrupt
  //Otherwise xAll could be triggered by quickly pressing multiple times when (by luck) a button is pressed when the timer fires
  //If you long-press both at the same time, let the checkButtons() logic determine an arbitrary winner
  
  if (digitalRead(upBtn) == LOW) {
    goUpAll = true;
  }

  if (digitalRead(downBtn) == LOW) {
    goDownAll = true;
  }

  ITimer0.stopTimer();
  //timer1.detach();
  return;
}

void checkButtons() {
  //https://forum.arduino.cc/t/button-pressed-more-than-2-seconds-do-somethingelse/136822/10
  
  static unsigned long lastAction = 0;
  static bool doUpLong = false;
  static bool doDownLong = false;

  //Debounce helps counter jittery button reads
  if (millis() - lastAction < DEBOUNCE_MS) {
    upPressed = false;
    upReleased = false;
    downPressed = false;
    downReleased = false;
    return;
  }
  
  if (goHalt) {
    dpln("halt");
    halt();
    dpln();
    goHalt = false;
    lastAction = millis();
    
  } else if (upPressed) { //UP HANDLERS
    if (handlingUp) { //EMI error, up hasn't been released
      upPressed = false;
      return;
    }
    handlingUp = true;
    
    timerStop();
    dpln("up pressed");
    upBtnPressed();
    dpln();
    
    timerStart();
    upPressed = false;
    
  } else if (goUpAll) { //up long press timer trigger
    goUpAll = false;
    dpln("upAll");
    upBtnLongPressed();
    dpln();

    lastAction = millis();
    upPressedLong = true;
    
  } else if (upReleased) { 
    if (!handlingUp) { //EMI error, up hasn't been pressed
      upReleased = false;
      return;
    }
    timerStop();

    if (upPressedLong) {
      dpln("up released long");
      upBtnLongReleased();
      upPressedLong = false;
    } else {
      dpln("up released short");
      upBtnReleased();
    }
    dpln();

    lastAction = millis();
    upReleased = false;
    handlingUp  = false;


    
  } else if (downPressed) { //DOWN HANDLERS
    if (handlingDown) { //EMI error, down hasn't been released
      downPressed = false;
      return;
    }
    handlingDown = true;
    
    timerStop();
    dpln("down pressed");
    downBtnPressed();
    dpln();
    
    downPressed = false;
    timerStart();
    
  } else if (goDownAll) { //down long press timer trigger
    goDownAll = false;
    dpln("downAll");
    downBtnLongPressed();
    dpln();

    lastAction = millis();
    downPressedLong = true;
    
  } else if (downReleased) { 
    if (!handlingDown) { //EMI error, down hasn't been pressed
      downReleased = false;
      return;
    }
    timerStop();

    if (downPressedLong) { //long press
      dpln("down released long");
      downBtnLongReleased();
      downPressedLong = false;
    } else {  //short press
      dpln("down released short");
      downBtnReleased();
    }
    dpln();

    lastAction = millis();
    downReleased = false;
    handlingDown = false;
  }
  
  return;
}
