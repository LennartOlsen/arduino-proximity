#include <SME_basic.h>

#include <Wire.h>
#include <VL6180.h>
#include <Arduino.h>
/*
  Proximity bad booooi
*/

unsigned long start;
String state;

unsigned long proximityInterval;
unsigned long lastProximityMillis;
int proximity;

unsigned long blinkInterval;
unsigned long lastBlinkMillis;
int blinkState = LOW;             // ledState used to set the LED

unsigned long blinkFastInterval;
unsigned long lastBlinkFastMillis;
int blinkFastState = LOW;             // ledState used to set the LED

unsigned long blinkRedInterval;
unsigned long lastBlinkRedMillis;
int blinkRedState = LOW;             // ledState used to set the LED

const long SECOND = 1000;
const String PROXIMITYCLOSE = "proximityclose";
const String PROXIMITYUNKOWN = "proximityunkown";
const String PROXIMITYFAR = "proximityfar";
const String WAITING = "waiting";

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();

  /** What the fuck? **/
  if (!smeProximity.begin()) {
    while (true); //endless loop due to error on VL6180 initialization
  }

  state = WAITING;
  proximityInterval = SECOND * 1;
  blinkInterval = SECOND * 2;
  blinkFastInterval = SECOND * 1;
  blinkRedInterval = SECOND * 2;
  
}

// the loop function runs over and over again forever
void loop() {
  doesState();
  transitionTest();
}

void transitionTest(){
  if(state == WAITING){
    if(proximity >= 255) { /** Proximity unkown **/
      doTransition(PROXIMITYUNKOWN);
      return;
    }
    if(proximity > 100 && proximity < 255){
      doTransition(PROXIMITYFAR);
      return;
    }
    if(proximity < 100){ /** Proximity close **/
       doTransition(PROXIMITYCLOSE);
       return;
    }
  }
  if(state == PROXIMITYUNKOWN) {
    if(proximity > 100 && proximity < 255){ /** Proximity close **/
       doTransition(WAITING);
       return;
    }
    if(proximity < 100){
      doTransition(WAITING);
      return;
    }
  }
  if(state == PROXIMITYCLOSE) {
    if(proximity >= 255) { /** Proximity unkown **/
      doTransition(WAITING);
      return;
    }
    if(proximity > 100 && proximity < 255){
       doTransition(WAITING);
       return;
    }
  }
  if(state == PROXIMITYFAR) {
    if(proximity == 255) { /** Proximity unkown **/
      doTransition(WAITING);
      return;
    }
    if(proximity < 100){ /** Proximity close **/
       doTransition(WAITING);
       return;
    }
  }  
}

void doTransition(String s){
  state = s;
  reset();
}

void doesState(){
  /** True for every state **/
  handleProximityInterval();
  
  if(state == WAITING){
    /** Do nothing **/
    return;
  }
  if(state == PROXIMITYUNKOWN){
    handleBlinkRedInterval();
  }
  if(state == PROXIMITYCLOSE){
    handleBlinkFastInterval();
  }
  if(state == PROXIMITYFAR){
    handleBlinkInterval();
  }
}

void reset(){
  blinkRedState = LOW;
  blinkFastState = LOW;
  blinkState = LOW;
  
  ledGreenLight(LOW);
  ledRedLight(LOW);
  ledBlueLight(LOW);
}

void handleProximityInterval(){
  unsigned long currentMillis = millis();

  if (currentMillis - lastProximityMillis >= proximityInterval) {
    // save the last time you blinked the LED
    lastProximityMillis = currentMillis;
    
    char ligth = smeProximity.rangePollingRead();

    if (ligth == 255) {
      SerialUSB.println("Infinity");
      proximity = 255;
      return;
    } else {
      int num = ligth + '0';
      SerialUSB.print(num);
      SerialUSB.println(" mm");
      proximity = num;
      return;
    }
  }
}


/** BLINKERS **/
void handleBlinkInterval(){
  unsigned long currentMillis = millis();

  if (currentMillis - lastBlinkMillis >= blinkInterval) {
    // save the last time you blinked the LED
    lastBlinkMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (blinkState == LOW) {
      blinkState = HIGH;
    } else {
      blinkState = LOW;
    }

    // set the LED with the ledState of the variable:
    ledGreenLight(blinkState);
  }
}

void handleBlinkFastInterval(){
  unsigned long currentMillis = millis();

  if (currentMillis - lastBlinkFastMillis >= blinkFastInterval) {
    // save the last time you blinked the LED
    lastBlinkFastMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (blinkFastState == LOW) {
      blinkFastState = HIGH;
    } else {
      blinkFastState = LOW;
    }

    // set the LED with the ledState of the variable:
    ledBlueLight(blinkFastState);
  }
}

void handleBlinkRedInterval(){
  unsigned long currentMillis = millis();

  if (currentMillis - lastBlinkRedMillis >= blinkRedInterval) {
    // save the last time you blinked the LED
    lastBlinkRedMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (blinkRedState == LOW) {
      blinkRedState = HIGH;
    } else {
      blinkRedState = LOW;
    }

    // set the LED with the ledState of the variable:
    ledRedLight(blinkRedState);
  }
}
