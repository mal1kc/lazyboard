#define kbd_tr_tr // define keyboard layout
#include "DigiKeyboard.h" // include library file

// ------ led control ------

#define led 1 // defining pin number

void setup() {
  pinMode(led, OUTPUT); // setting pinmode as output
}

void loop() {
  digitalWrite(led, HIGH); // led on
  delay(500);
  digitalWrite(led, LOW); // led off
  delay(500);
}

// setting tr(turkish) keyboard layout
#define kbd_tr_tr
#include "DigiKeyboard.h"

void setup() {
  // disable input for 2,5 second for no startup errors
  // and sending empty keystroke
  DigiKeyboard.delay(2500);
  DigiKeyboard.sendKeyStroke(0);
}

void loop() {

  // reading mechanic switch data and assigning into data variable
  int data = analogRead(1);
  if ( data > 100)
  {
    // writing "read val: " from keyboard
    // WARN: error from library cannot print 'ğ' keyword
    DigiKeyboard.print("read val:    ");
    DigiKeyboard.print(data);
    // sending 'return'/'enter' key stroke
    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    while (analogRead(1) > 100);
  }
  // for ignoring analog read unstabilty to waiting very short time
  DigiKeyboard.delay(50);
}
