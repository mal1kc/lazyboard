// ---- switch control -----

#define kbd_tr_tr
#include "DigiKeyboard.h"

void setup() {
  DigiKeyboard.delay(2500);
  DigiKeyboard.sendKeyStroke(0);
}

void loop() {

  int data = analogRead(1);
  if ( data > 100)
  {
      //if button 6 pressed we do this operations
    if (data > 933) {
      DigiKeyboard.print("button 6 pressed");
    }
      //if button 5 pressed we do this operations
    else if (data > 747) {
      DigiKeyboard.print("button 5 pressed");
    }
      //if button 4 pressed we do this operations
    else if (data > 577) {
      DigiKeyboard.print("button 4 pressed");
    }
      //if button 3 pressed we do this operations
    else if (data > 421) {
      DigiKeyboard.print("button 3 pressed");
    }
      //if button 2 pressed we do this operations
    else if (data > 249) {
      DigiKeyboard.print("button 2 pressed");
    }
      //if button 1 pressed we do this operations
    else  {
      DigiKeyboard.print("button 1 pressed");
    }

    DigiKeyboard.sendKeyStroke(KEY_ENTER);
    while (analogRead(1) > 100);
  }

}
