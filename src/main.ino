#define kbd_tr_tr
#include "DigiKeyboard.h"

// s1 read val:    161
// s2 read val:    336
// s3 read val:    506
// s4 read val:    658
// s5 read val:    838
// s6 read val:    1023
// button 1 pressed
// button 2 pressed
// button 3 pressed
// button 4 pressed
// button 5 pressed
// button 6 pressed
//

void setup() {
  // disable input for 2,5 second for no startup errors
  // and sending empty keystroke
  DigiKeyboard.delay(2500);
  DigiKeyboard.sendKeyStroke(0);
}

void loop() {

  int data = analogRead(1);
  if (data > 100) {
    // if button 6 pressed we do this operations
    if (data > 1022) {
      DigiKeyboard.print("button 6 pressed");
    }
    // if button 5 pressed we do this operations
    else if (data > 837) {
      DigiKeyboard.print("button 5 pressed");
    }
    // if button 4 pressed we do this operations
    else if (data > 658) {
      DigiKeyboard.print("button 4 pressed");
    }
    // if button 3 pressed we do this operations
    else if (data > 504) {
      DigiKeyboard.print("button 3 pressed");
    }
    // if button 2 pressed we do this operations
    else if (data > 333) {
      DigiKeyboard.print("button 2 pressed");
    }
    // if button 1 pressed we do this operations
    else {
      DigiKeyboard.print("button 1 pressed");
    }

    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    while (analogRead(1) > 100)
      ;
  }

  // for ignoring analog read unstabilty to waiting very short time
  DigiKeyboard.delay(50);
}
