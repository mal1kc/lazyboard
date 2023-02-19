#define kbd_tr_tr
#include "DigiKeyboard.h"

// setting IR reader pin, 0 is default for 'Lazyboard'
const byte IRpin = 0;

// creating and assinging remote and irCode variables ,there are neccesary for our algorithm
volatile boolean remote = false;
volatile unsigned long irCode = 0;

void setup() {

    // setting IR reader pin's mode as Input
  pinMode(IRpin, INPUT);


  // waiting to 2,5s to initialize and start with empty keystroke
  DigiKeyboard.delay(2500);
  DigiKeyboard.sendKeyStroke(0);
}

void loop() {
    // IR reader gives 1 (HIGH) when it not detect any signal
  if (digitalRead(IRpin)) {
    remoting();
  } else {
    // if pressed 1.button
    if (irCode == 0xE718FF00) {
      DigiKeyboard.println("Up");
      irCode = 0;
    }
    // if pressed 2.button
    else if(irCode == 0xAD52FF00)
    {
      DigiKeyboard.println("Down");
      irCode = 0;
    }
  // for our algorith work we change remote as true
  remote = true;
}


//IR reading func
void remoting()
{
  if (remote) {
    remote = false;
    unsigned long T;
    for (byte n = 0; n < 32; n++) {
      do {
        T = pulseIn(IRpin, HIGH, 2200);
      } while (T < 64);
      bitWrite(irCode, n, T > 1120);
    }
  }
}
