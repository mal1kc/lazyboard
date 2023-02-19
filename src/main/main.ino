// setting tr(turkish) keyboard layout
#define kbd_tr_tr
#include "DigiKeyboard.h"

// vals of buttons 
// S1 = read val:    161
// S2 = read val:    336
// S3 = read val:    506
// S4 = read val:    659
// S5 = read val:    838
// S6 = read val:    1023



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
