#ifndef BUTTONS_H
#define BUTTONS_H

#define TIMER_INTERVAL_2S 2000
//#define TIMER0_INTERVAL_US 1000 //in practice this seems to be about half as long as it should be
#define TIMER0_INTERVAL_US 625 //for 160MHz processor @ TIM_DIV256; halve for 80MHz mode
#define DEBOUNCE_MS 150

//Button pins
//https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
const int upBtn = D1;
const int haltBtn = D2;
const int downBtn = D3; //permenantly pulled up, which is fine because we want that
//In hindsight using D5-D7 would be a better general-purpose choice

//Button states
volatile bool handlingUp = false;
volatile bool upPressed = false;
volatile bool upReleased = false;
volatile bool handlingDown = false;
volatile bool downPressed = false;
volatile bool downReleased = false;
volatile bool goHalt = false;
volatile bool goUpAll = false;
volatile bool goDownAll = false;

volatile bool upPressedLong = false;
volatile bool downPressedLong = false;

//Setup
void timerSetup();
void buttonSetup();

//Inturrupt routines
void ICACHE_RAM_ATTR INT_UP();
void ICACHE_RAM_ATTR INT_HALT();
void ICACHE_RAM_ATTR INT_DOWN();
void ICACHE_RAM_ATTR timerHandler();

//Main logic
void checkButtons();

#endif
